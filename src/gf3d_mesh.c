#include <stdalign.h>
#include <SDL2/SDL_stdinc.h>
#include <gf3d_pipeline.h>
#include "simple_logger.h"
#include "gfc_types.h"
#include "gfc_shape.h"
#include "gf3d_swapchain.h"
#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_commands.h"
#include "gf2d_sprite.h"
#include "gf3d_buffers.h"
#include "gf3d_camera.h"
#include "gf3d_obj_load.h"
#include "gf3d_mesh.h"


// The pipeline is not being cleared aka the pipeline is not being run?
// Mouse trail is gone now?

//gf3d_mesh.c:271: In create vertex buf: Buffersize 347040, vertexCount: 10845
// versus 
//gf3d_mesh.c:264: In meshload facecount: 21332, vertexcount: 63996
//gf3d_mesh.c:267: V[0]=(-1.39,1.84,-1.06)
//gf3d_mesh.c:267: V[1]=(-1.72,1.82,-1.03)
//gf3d_mesh.c:267: V[2]=(-1.71,1.90,-0.97)

#define MESH_ATTRIBUTE_COUNT 3

/*
    WAIT so like each primitive has a vertexbuffer and stuff??
    In 
        gf3d_mesh_primitive_queue_render
    you need to already have the things for the prims
    and you get those somewhere....???
*/

extern int __DEBUG;

typedef struct {
    Mesh            *mesh_list;
    Uint32          mesh_count;
    Uint32          chain_length;     /**<length of swap chain*/
    VkDevice        device;           /**<logical vulkan device*/
    Pipeline        *pipe;             /**<the pipeline associated with sprite rendering*/
    Pipeline        *sky_pipe; 
    VkBuffer        faceBuffer;       /**<memory handle for the face buffer (always two faces)*/
    VkDeviceMemory  faceBufferMemory; /**<memory habdle for the face memory*/
    //Do we need vertex buffers?
    VkVertexInputAttributeDescription   attributeDescriptions[MESH_ATTRIBUTE_COUNT];
    VkVertexInputBindingDescription     bindingDescription;
    Texture         *defaultTexture;
    // Default texture?
} MeshManager;
//Free mesh?
void gf3d_mesh_primitive_create_vertex_buffers(MeshPrimitive *prim);
void gf3d_mesh_setup_face_buffers(MeshPrimitive *prim);
MeshPrimitive *gf3d_mesh_primitive_new();
void gf3d_mesh_manager_close();
void gf3d_mesh_free(Mesh *mesh);
void gf3d_mesh_delete(Mesh *mesh);
void gf3d_mesh_queue_render(Mesh *mesh,Pipeline *pipe, void  *uboData, Texture *texture);
VkVertexInputAttributeDescription * gf3d_mesh_get_attribute_descriptions(Uint32 *count);
void gf3d_mesh_primitive_queue_render(MeshPrimitive *prim,Pipeline *pipe,void *uboData,Texture *texture);
Mesh *gf3d_mesh_get_by_filename(const char *filename);

static MeshManager mesh_manager = {0};

void gf3d_mesh_draw(Mesh *mesh, GFC_Matrix4 modelMat, GFC_Color mod, Texture *texture, GFC_Vector3D lightPos, GFC_Color lightColor){

    MeshUBO ubo = {0};

    if (!mesh)return;
    gfc_matrix4_copy(ubo.model,modelMat);
    gf3d_vgraphics_get_view(&ubo.view);
    gf3d_vgraphics_get_projection_matrix(&ubo.proj);

    ubo.color = gfc_color_to_vector4f(mod);
    ubo.lightColor = gfc_color_to_vector4f(lightColor);
    ubo.lightPos = gfc_vector3dw(lightPos,1.0);
    ubo.camera = gfc_vector3dw(gf3d_camera_get_position(),1.0);
    gf3d_mesh_queue_render(mesh, mesh_manager.pipe,&ubo, texture);
}

void gf3d_sky_draw(Mesh *mesh, GFC_Matrix4 modelMat, GFC_Color mod, Texture *texture){

    SkyUBO ubo = {0};

    if (!mesh)return;
    gfc_matrix4_copy(ubo.model,modelMat);
    gf3d_vgraphics_get_view(&ubo.view);
    ubo.view[0][3] = 0;
    ubo.view[1][3] = 0;
    ubo.view[2][3] = 0;
    ubo.view[3][0] = 0;
    ubo.view[3][1] = 0;
    ubo.view[3][2] = 0;

    gf3d_vgraphics_get_projection_matrix(&ubo.proj);

    //for the matrix make sure its zeroed


    ubo.color = gfc_color_to_vector4f(mod);
    gf3d_mesh_queue_render(mesh, mesh_manager.sky_pipe,&ubo, texture);
}
void gf3d_mesh_queue_render(Mesh *mesh,Pipeline *pipe, void  *uboData, Texture *texture)
{
    int i,c;
    MeshPrimitive *prim;
    if ((!mesh)|| (!pipe) || (!uboData))return;
    c = gfc_list_count(mesh->primitives);
    for (i = 0;i < c; i++)
    {
        prim = gfc_list_nth(mesh->primitives,i);
        if (!prim)continue;
        gf3d_mesh_primitive_queue_render(prim,pipe,uboData,texture);
    }
}
void gf3d_mesh_primitive_queue_render(MeshPrimitive *prim,Pipeline *pipe,void *uboData,Texture *texture)
{
    if ((!prim)||(!pipe)||(!uboData))return;
    if (!texture)texture = mesh_manager.defaultTexture;
    gf3d_pipeline_queue_render(
        pipe,
        prim->vertexBuffer,
        prim->vertexCount,
        prim->faceBuffer,
        uboData,
        texture);
}
VkVertexInputBindingDescription * gf3d_mesh_manager_get_bind_description()
{
    mesh_manager.bindingDescription.binding = 0;
    mesh_manager.bindingDescription.stride = sizeof(Vertex);
    mesh_manager.bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return &mesh_manager.bindingDescription;
}
VkVertexInputAttributeDescription * gf3d_mesh_get_attribute_descriptions(Uint32 *count)
{
    mesh_manager.attributeDescriptions[0].binding = 0;
    mesh_manager.attributeDescriptions[0].location = 0;
    mesh_manager.attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    mesh_manager.attributeDescriptions[0].offset = offsetof(Vertex, vertex);

    mesh_manager.attributeDescriptions[1].binding = 0;
    mesh_manager.attributeDescriptions[1].location = 1;
    mesh_manager.attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    mesh_manager.attributeDescriptions[1].offset = offsetof(Vertex, normal);

    mesh_manager.attributeDescriptions[2].binding = 0;
    mesh_manager.attributeDescriptions[2].location = 2;
    mesh_manager.attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    mesh_manager.attributeDescriptions[2].offset = offsetof(Vertex, texel);
    if (count)* count = MESH_ATTRIBUTE_COUNT;
    return mesh_manager.attributeDescriptions;
}
void gf3d_mesh_init(Uint32 mesh_max)
{
    Uint32 count = 0;
    if (mesh_max == 0)
    {
        slog("cannot initalize mesh manager for 0 sprites");
        return;
    }
    mesh_manager.chain_length = gf3d_swapchain_get_chain_length();
    mesh_manager.mesh_list = (Mesh *)gfc_allocate_array(sizeof(Mesh),mesh_max);
    mesh_manager.mesh_count = mesh_max;
    mesh_manager.device = gf3d_vgraphics_get_default_logical_device();

    gf3d_mesh_get_attribute_descriptions(&count);

    //Sky pipeline- it should be before 
    mesh_manager.sky_pipe = gf3d_pipeline_create_from_config(
        gf3d_vgraphics_get_default_logical_device(),
        "config/sky_pipeline.cfg",
        gf3d_vgraphics_get_view_extent(),
        mesh_max,
        gf3d_mesh_manager_get_bind_description(),
        gf3d_mesh_get_attribute_descriptions(NULL),
        count,
        sizeof(SkyUBO),
        VK_INDEX_TYPE_UINT16
    );
    mesh_manager.pipe = gf3d_pipeline_create_from_config(
        gf3d_vgraphics_get_default_logical_device(),
        "config/model_pipeline.cfg",
        gf3d_vgraphics_get_view_extent(),
        mesh_max,
        gf3d_mesh_manager_get_bind_description(),
        gf3d_mesh_get_attribute_descriptions(NULL),
        count,
        sizeof(MeshUBO),
        VK_INDEX_TYPE_UINT16
    );
    


    mesh_manager.defaultTexture = gf3d_texture_load("images/default.png");
    if(__DEBUG)slog("mesh manager initiliazed");
    //atexit(gf3d_mesh_manager_close);
}
void gf3d_mesh_primitive_create_vertex_buffers(MeshPrimitive *prim)
{
    void *data = NULL;
    VkDevice device = gf3d_vgraphics_get_default_logical_device();
    Vertex *vertices;
    Uint32 vcount;
    size_t bufferSize;
    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

    if (!prim)
    {
        slog("no mesh primitive provided");
        return;
    }

    vertices = prim->objData->faceVertices;
    vcount = prim->objData->face_vert_count;
    bufferSize = sizeof(Vertex) * vcount;

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices, (size_t) bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT|VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &prim->vertexBuffer, &prim->vertexBufferMemory);

    gf3d_buffer_copy(stagingBuffer, prim->vertexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);

    prim->vertexCount = vcount;

}
void gf3d_mesh_setup_face_buffers(MeshPrimitive *prim)
{
    void *data = NULL;
    Face *faces = NULL;
    Uint32 fcount;

    VkDevice device = gf3d_vgraphics_get_default_logical_device();
    VkDeviceSize bufferSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    if ((!prim)||(!prim->objData))return;
    faces = prim->objData->outFace;
    fcount = prim->objData->face_count;
    if ((!faces)||(!fcount))return;
    bufferSize = sizeof(Face) *fcount;

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, faces, (size_t) bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &prim->faceBuffer, &prim->faceBufferMemory);

    gf3d_buffer_copy(stagingBuffer, prim->faceBuffer, bufferSize);

    prim->faceCount = fcount;
    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);
}
//From here is my functions
Pipeline *gf3d_mesh_get_pipeline(){
    return mesh_manager.pipe;

}

Mesh *gf3d_mesh_load(const char *filename){
    MeshPrimitive *prim; 
    Mesh *mesh;
    ObjData *objdata;

    mesh = gf3d_mesh_get_by_filename(filename);
    if (mesh)
    {
        mesh->_refCount++;
        return mesh; 
    } 


    objdata = gf3d_obj_load_from_file(filename);
    if(!objdata){ slog("failed to obj"); return NULL;}
    mesh = gf3d_mesh_new();
    if(!mesh){
        slog("no mesh in mesh load"); 
        gf3d_obj_free(objdata);
        return NULL;
    }
    prim = gf3d_mesh_primitive_new();
    if(!prim){
        slog("no prim in mesh load"); 
        free(objdata);
        gf3d_mesh_free(mesh);
        return NULL;
    }

    // Per prim...?
    //slog("Per prim");
    prim->objData = objdata;
    
    gf3d_mesh_primitive_create_vertex_buffers(prim);
    gf3d_mesh_setup_face_buffers(prim);
    //slog("In meshload facecount: %i, vertexcount: %i", prim->faceCount, prim->vertexCount);
    /*Vertex *vdata = prim->objData->faceVertices;
    for (int i = 0; i <3 && i< prim->vertexCount; i++){
        //slog("V[%d]=(%.2f,%.2f,%.2f)", i, vdata[i].vertex.x, vdata[i].vertex.y, vdata[i].vertex.z);
    }
    Face *fdata = prim->objData->outFace;
    for (int i = 0; i <3 && i< prim->faceCount; i++){
        //slog("V[%d]=(%.2f,%.2f,%.2f)", i, fdata[i].verts); //fdata[i].vertex.y, fdata[i].vertex.z);
    }
    */
    //slog("After per prim, 1");
    gfc_list_append(mesh->primitives, prim);
    return mesh;
    
    // chail length is swapchain gfc_edge_length
}

Mesh *gf3d_mesh_new(){
    int i;
    for(i = 0; i < mesh_manager.mesh_count ; i++){
            slog("continue");

        if(mesh_manager.mesh_list[i]._refCount) {continue;}
        memset (&mesh_manager.mesh_list[i], 0, sizeof(Mesh)); 
        mesh_manager.mesh_list[i]._refCount = 1;
        mesh_manager.mesh_list[i].primitives = gfc_list_new(); 
        //If you successfuly load the primitive, append it to the list
        return &mesh_manager.mesh_list[i];
    }
    slog("gf3d_mesh_new: no free slots for new meshes");
    return NULL;
}

MeshPrimitive *gf3d_mesh_primitive_new(){ 
    return gfc_allocate_array(sizeof(MeshPrimitive),mesh_manager.mesh_count);
}

void gf3d_mesh_free(Mesh *mesh)
{
    if (!mesh)return;
    mesh->_refCount--;
    if (mesh->_refCount <= 0)gf3d_mesh_delete(mesh);
    ///This seems wrong
}


Mesh *gf3d_mesh_get_by_filename(const char *filename){
    int i;
    if (!filename) return NULL;
    for(i = 0; i < mesh_manager.mesh_count; i++){
        if(!mesh_manager.mesh_list[i]._refCount) continue;
        if(gfc_line_cmp(mesh_manager.mesh_list[i].filename, filename) == 0){
            return &mesh_manager.mesh_list[i];
        } 
    }
    return NULL;
}


void gf3d_mesh_delete(Mesh *mesh)
{
    if (!mesh)return;
    /*

    TODO free up objdata

    
    if (mesh->buffer != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(gf2d_sprite.device, sprite->buffer, NULL);
    }
    if (mesh->bufferMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(gf2d_sprite.device, sprite->bufferMemory, NULL);
    }
    */
    //gf3d_texture_free(mesh->texture);
    memset(mesh,0,sizeof(Mesh));
}

/*void gf3d_mesh_primitive_create_vertex_buffers(MeshPrimitive *prim);
void gf3d_mesh_setup_face_buffers(MeshPrimitive *prim, Face *faces, Uint32 fcount);
MeshPrimitive *gf3d_mesh_primitive_new();
void gf3d_mesh_manager_close();
void gf3d_mesh_free(Mesh *mesh);
void gf3d_mesh_delete(Mesh *mesh);


// forward declare a bunch of stuff

Mesh *gf3d_mesh_new(){
    int i;
    for(i = 0; i < mesh_manager.mesh_count ; i++){
        if(mesh_manager.mesh_list[i]._refCount) continue;
        memset (&mesh_manager.mesh_list[i], 0, sizeof(Mesh)); 
        mesh_manager.mesh_list[i]._refCount = 1;
        mesh_manager.mesh_list[i].primitives = gfc_list_new(); 
        //If you successfuly load the primitive, append it to the list
        return &mesh_manager.mesh_list[i];
    }
    slog("gf3d_mesh_new: no free slots for new meshes");
    return NULL;
}

Mesh *gf3d_mesh_get_by_filename(const char *filename){
    int i;
    if (!filename) return NULL;
    for(i = 0; i < mesh_manager.mesh_count; i++){
        if(!mesh_manager.mesh_list[i]._refCount) continue;
        if(gfc_line_cmp(mesh_manager.mesh_list[i].filename, filename) == 0){
            return &mesh_manager.mesh_list[i];
        } 
    }
    return NULL;
}

Mesh *gf3d_mesh_load(const char *filename){
    MeshPrimitive *prim; 
    Mesh *mesh;
    ObjData *objdata;

    mesh = gf3d_mesh_get_by_filename(filename);
    if (mesh)
    {
        mesh->_refCount++;
        return mesh; 
    } 


    objdata = gf3d_obj_load_from_file(filename);
    if(!objdata){ slog("failed to obj"); return NULL;}
    mesh = gf3d_mesh_new();
    if(!mesh){
        slog("no mesh in mesh load"); 
        gf3d_obj_free(objdata);
        return NULL;
    }
    prim = gf3d_mesh_primitive_new();
    if(!prim){
        slog("no prim in mesh load"); 
        free(objdata);
        gf3d_mesh_free(mesh);
        return NULL;
    }

    // Per prim...?
    slog("Per prim");
    prim->objData = objdata;
    
    gf3d_mesh_primitive_create_vertex_buffers(prim);
    gf3d_mesh_setup_face_buffers(prim, objdata->outFace, objdata->face_count);
    slog("In meshload facecount: %i, vertexcount: %i", prim->faceCount, prim->vertexCount);
    Vertex *vdata = prim->objData->faceVertices;
    for (int i = 0; i <3 && i< prim->vertexCount; i++){
        slog("V[%d]=(%.2f,%.2f,%.2f)", i, vdata[i].vertex.x, vdata[i].vertex.y, vdata[i].vertex.z);
    }
    Face *fdata = prim->objData->outFace;
    for (int i = 0; i <3 && i< prim->faceCount; i++){
        slog("V[%d]=(%.2f,%.2f,%.2f)", i, fdata[i].verts); //fdata[i].vertex.y, fdata[i].vertex.z);
    }
    
    //slog("After per prim, 1");
    gfc_list_append(mesh->primitives, prim);
    return mesh;
    
    // chail length is swapchain gfc_edge_length
}MeshPrimitive *gf3d_mesh_primitive_new(){ 
    return gfc_allocate_array(sizeof(MeshPrimitive),mesh_manager.mesh_count);
}

void gf3d_mesh_manager_init(Uint32 mesh_max)
{
    //void* data;
    Uint32 count = 0;
    //SpriteFace faces[2];
    //size_t bufferSize;    
    //VkBuffer stagingBuffer;
    // VkDeviceMemory stagingBufferMemory;

    if (mesh_max == 0)
    {
        slog("cannot initialize mesh manager for 0 meshes");
        return;
    }
    //bufferSize = sizeof(Vertex);
    //chail length is swapchain gfc_edge_length 
    // what does this mean 
    mesh_manager.chain_length = gf3d_swapchain_get_chain_length();
    mesh_manager.mesh_list = (Mesh *)gfc_allocate_array(sizeof(Mesh),mesh_max);
    mesh_manager.mesh_count = mesh_max;
    mesh_manager.device = gf3d_vgraphics_get_default_logical_device();

    //count = vertexattribuecount
    // gf3d_mesh_get_attribute_descriptions(&count);
    slog("Trying to initalize the mesh manager");
    //Its this that gives the mesh manager its pipe
    //gf2d_sprite_get_attribute_descriptions(&count);
        
    // count goes from 0 to 2
    VkVertexInputAttributeDescription *attrDesc = gf3d_mesh_get_attribute_descriptions(&count);
    VkVertexInputBindingDescription *bindDesc = gf3d_mesh_get_bind_description();
    slog("COUNT??? %i", count);
    mesh_manager.pipe = gf3d_pipeline_create_from_config(
        gf3d_vgraphics_get_default_logical_device(),
        "config/overlay_pipeline.cfg",
        gf3d_vgraphics_get_view_extent(),
        mesh_max, //1024
        bindDesc,
        attrDesc,
        count, // count -> sprite attribute count
        sizeof(MeshUBO),
        VK_INDEX_TYPE_UINT16 //Very large models may need to be updated to be bigger?
    );     
    //slog("Mesh Max: %i",mesh_max); //Should you still be 2?
    slog("mesh manager initalized");
    
    //It never runs this?  
    atexit(gf3d_mesh_manager_close);
}


void gf3d_mesh_init(Uint32 mesh_max){
    gf3d_mesh_manager_init(mesh_max);
    //gfc_allocate_array(mesh_)
    //mesh_manager.chain_length = gfc_edge_length();
    manager chain gfc_edge_length
manager mesh list allocate
????
}

void gf3d_mesh_setup_face_buffers(MeshPrimitive *prim, Face *faces, Uint32 fcount){
    void *data = NULL;
    size_t bufferSize;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    //faces = prim->objData->outFace;
    //fcount = prim->objData->face_count;
    bufferSize = sizeof(Face) * fcount;

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);
    vkMapMemory(mesh_manager.device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, faces, (size_t) bufferSize);
    vkUnmapMemory(mesh_manager.device, stagingBufferMemory);

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &prim->faceBuffer, &prim->faceBufferMemory);

    gf3d_buffer_copy(stagingBuffer, prim->faceBuffer, bufferSize);

    vkDestroyBuffer(mesh_manager.device, stagingBuffer, NULL);
    vkFreeMemory(mesh_manager.device, stagingBufferMemory, NULL); 
    //slog("in face buffer end");
    prim->faceCount = fcount;

    //prim->faceBuffer = data;
}


void gf3d_mesh_primitive_create_vertex_buffers(MeshPrimitive *prim)
{
     facecount, vertcount, face list and face count?, buffersize

        Face buffer vs vertex buffers???? I copy pasted face to here


        OK so now i have a very messy create vertex buffer that uses faceVerticies, but even now im not so sure what to do 
    
    void *data = NULL;
    Vertex *vertices;
    Uint32 vcount;
    size_t bufferSize;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    vertices = prim->objData->faceVertices; //!!!!!!!!!!!! outface is for faces
    vcount = prim->objData->vertex_count; //basically vertecies and vcount
    //slog("fcount=%u", prim->objData->vertex_count);
    for (int i = 0; i <3 && i< fcount; i++){
        slog("V[%d]=(%f,%f,%f)", i, faces[i].vertex.x, faces[i].vertex.y, faces[i].vertex.z);
    }

    bufferSize = sizeof(Vertex) * vcount; //vertex -> face -> vertex
    
    
    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);
    vkMapMemory(mesh_manager.device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices, (size_t) bufferSize);

    vkUnmapMemory(mesh_manager.device, stagingBufferMemory);
    //VK_BUFFER_USAGE_INDEX_BUFFER_BIT -> VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
    // index for face, vertex for vertex
    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &prim->vertexBuffer, &prim->vertexBufferMemory);
  

    gf3d_buffer_copy(stagingBuffer, prim->vertexBuffer, bufferSize);

    vkDestroyBuffer(mesh_manager.device, stagingBuffer, NULL);
    vkFreeMemory(mesh_manager.device, stagingBufferMemory, NULL);    

    //make sure the primitive has everything it needs
    //prim->vertexBuffer = data;
    //prim->vertexCount = vcount;//
    prim->vertexCount  = vcount;
    slog("In create vertex buf: Buffersize %i, vertexCount: %i", bufferSize, prim->vertexCount);
}

VkVertexInputBindingDescription * gf3d_mesh_get_bind_description()
{
    mesh_manager.bindingDescription.binding = 0;
    mesh_manager.bindingDescription.stride = sizeof(Vertex);
    mesh_manager.bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return &mesh_manager.bindingDescription;
}


void gf3d_mesh_manager_close()
{
    int i;
    for (i = 0; i < mesh_manager.mesh_count;i++)
    {
        //gf2d_sprite_delete(&mesh_manager.mesh_list[i]);
        gf3d_mesh_delete(&mesh_manager.mesh_list[i]);
    }
    if (mesh_manager.mesh_list)
    {
        free(mesh_manager.mesh_list);
    }
    if (mesh_manager.faceBuffer != VK_NULL_HANDLE)
    {
        //CHANGE!! 
        vkDestroyBuffer(mesh_manager.device, mesh_manager.faceBuffer, NULL);
    }
    if (mesh_manager.faceBufferMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(mesh_manager.device, mesh_manager.faceBufferMemory, NULL);
    }

    memset(&mesh_manager,0,sizeof(MeshManager));
    if(__DEBUG)slog("mesh manager closed");
    // free the default texture
}

VkVertexInputAttributeDescription * gf3d_mesh_get_attribute_descriptions(Uint32 *count)
{
    mesh_manager.attributeDescriptions[0].binding = 0;
    mesh_manager.attributeDescriptions[0].location = 0;
    mesh_manager.attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    mesh_manager.attributeDescriptions[0].offset = offsetof(Vertex, vertex);

    mesh_manager.attributeDescriptions[1].binding = 0;
    mesh_manager.attributeDescriptions[1].location = 1;
    mesh_manager.attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    mesh_manager.attributeDescriptions[1].offset = offsetof(Vertex, normal);
    
    mesh_manager.attributeDescriptions[2].binding = 0;
    mesh_manager.attributeDescriptions[2].location = 2;
    mesh_manager.attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    mesh_manager.attributeDescriptions[2].offset = offsetof(Vertex, texel);
    if (count)c = MESH_ATTRIBUTE_COUNT; //Were leaving this as sprite because IDK
    return mesh_manager.attributeDescriptions;
}

//primitive_render => primitive_queue_render
void gf3d_mesh_primitive_queue_render(MeshPrimitive *prim,Pipeline *pipe,void *uboData,Texture *texture){

    if((!prim) || (!pipe) || (!uboData)) return;
    //if(!texture) texure = mesh_manager.default //DEFAULT TEXTURE?
    
    //if(prim->faceBuffer){slog("yes facebuffer???");}
    //if(prim->vertexBuffer){slog("yes vertexbuffer???");}
    //if(prim->vertexCount){slog("yes vcount???");}

    gf3d_pipeline_queue_render(
        pipe,
        prim->vertexBuffer, 
        prim->vertexCount,
        prim->faceBuffer,
        uboData,
        texture);
    //This is being run appropriately

    //slog("In primitive queue render: %i, %p, %p",prim->vertexCount, (unsigned long long)prim->vertexBuffer, (unsigned long long)prim->faceBuffer);
}


void gf3d_mesh_queue_render(Mesh *mesh, Pipeline *pipe, void *uboData, Texture *texture){
    // primitives are whats being rendered
    // We queue them up to render, not the mesh
    // This is mesh queue, takes the mesh and for every prim it puts it to render
    int i, c;
    MeshPrimitive *prim;
    if((!mesh) || (!pipe) || (!uboData)) return;
    c = gfc_list_count(mesh->primitives);
    for(i = 0; i < c; i++){
        prim = gfc_list_nth(mesh->primitives,i); //comma i? not dot i dummy
        if(!prim) continue;
        
        gf3d_mesh_primitive_queue_render(prim, pipe, uboData, texture);
    }
}

void gf3d_mesh_draw(Mesh * mesh, GFC_Matrix4 modelMat, GFC_Color mod, Texture *texture){
    MeshUBO ubo = {0};
    if(!mesh) return;
    
    // slog("draw");

    gfc_matrix4_copy(ubo.model, modelMat);

    gf3d_vgraphics_get_view(&ubo.view);
    gf3d_vgraphics_get_projection_matrix(&ubo.proj);
    
    ubo.color = gfc_color_to_vector4f(mod);
    ubo.camera = gfc_vector3dw(gf3d_camera_get_position(), 1.0);//camera get position
    
    gf3d_mesh_queue_render(mesh, mesh_manager.pipe, &ubo, texture);
            
    //GFC_Vector4D new4d = gfc_color_to_vector4f(mod);
    // gfc_color_copy(&ubo.color, mod);
    // todo color
    //gfc_color_copy(ubo.color, new4d);
    
    
    //Mesh vs pipeline queue renderer?? 
    //renderer => render
    
    
    //if(!texture) texture = mesh_manager // load the default texture in the manager not the draw manager.default

    //if(!texture) texure = mesh_manager.de //default again
    gf3d_pipeline_queue_render(
        pipe
        prim->vertexBuffer, 
        prim
        ..
        uboData
        texture
    )
}



Pipeline *gf3d_mesh_get_pipeline(){
    return mesh_manager.pipe;

}

void gf3d_mesh_delete(Mesh *mesh)
{
    if (!mesh)return;
    

    TODO free up objdata

    
    if (mesh->buffer != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(gf2d_sprite.device, sprite->buffer, NULL);
    }
    if (mesh->bufferMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(gf2d_sprite.device, sprite->bufferMemory, NULL);
    }
    /
    //gf3d_texture_free(mesh->texture);
    memset(mesh,0,sizeof(Mesh));
}

void gf3d_mesh_free(Mesh *mesh)
{
    if (!mesh)return;
    mesh->_refCount--;
    if (mesh->_refCount <= 0)gf3d_mesh_delete(mesh);
    ///This seems wrong
}
*/


/*
    Current great mysteries
    What the hell is the obj data?
    What the hell is SpriteVertex
        its just Vertex
*/