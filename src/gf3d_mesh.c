#include <stdalign.h>
#include <SDL2/SDL_stdinc.h>
#include <gf3d_pipeline.h>
#include "simple_logger.h"
#include "gfc_types.h"
#include "gfc_shape.h"
#include "gf3d_buffers.h"
#include "gf3d_swapchain.h"
#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_commands.h"
#include "gf2d_sprite.h"
#include "gf3d_buffers.h"
#include "gf3d_camera.h"
#include "gf3d_obj_load.h"
#include "gf3d_mesh.h"



#define SPRITE_ATTRIBUTE_COUNT 2

/*
    Forward declarations- what functions are actually being used
    I kinda get the draw => mesh queue => prim queue but not much else
    what the hell was the first class
    mesh_new(), filename I think I got, and most of class 2
    but there was the back half of the first class
    specifically gfc3d_mesh_load and gfc3d_mesh_primitive_new and gf3d_mesh_manager_init


    Ok 10.3 where do we put the face buffers and vertex buffers
    Where is mesh_manager_init called????
*/

//Load, primitive_new, and manager_init
extern int __DEBUG;

typedef struct {
    Mesh            *mesh_list;
    Uint32          mesh_count;
    Uint32          chain_length;     /**<length of swap chain*/
    VkDevice        device;           /**<logical vulkan device*/
    Pipeline        *pipe;             /**<the pipeline associated with sprite rendering*/
    VkBuffer        faceBuffer;       /**<memory handle for the face buffer (always two faces)*/
    VkDeviceMemory  faceBufferMemory; /**<memory habdle for tge face memory*/
    //Do we need vertex buffers?
    VkVertexInputAttributeDescription   attributeDescriptions[SPRITE_ATTRIBUTE_COUNT];
    VkVertexInputBindingDescription     bindingDescription;
    // Default texture?
} MeshManager;
//Free mesh?

static MeshManager mesh_manager = {0};

void gf3d_mesh_primitive_create_vertex_buffers(MeshPrimitive *prim);
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
        // add it to the thing
        //MeshPrimitive *prim = gf3d_mesh_primitive_new();
        //if(!prim){ slog("Failed to create the prim in mesh_new");return;}

        memset (&mesh_manager.mesh_list[i], 0, sizeof(Mesh)); // 0 as the second parameter? not prim?
        mesh_manager.mesh_list[i]._refCount = 1;
        mesh_manager.mesh_list[i].primitives = gfc_list_new(); 
        //If you successfuly load the primitive, append it to the list
        return &mesh_manager.mesh_list[i];
    }
    slog("gf3d_mesh_new: no free slots for new meshes");
    return NULL;

    /*    int i;
    for (i = 0; i < gf2d_sprite.max_sprites; i++)
    {
        if (gf2d_sprite.sprite_list[i]._inuse)continue;
        gf2d_sprite.sprite_list[i]._inuse = 1;
        return &gf2d_sprite.sprite_list[i];
    }
    slog("gf2d_sprite_new: no free slots for new sprites");
    return NULL;
    */
}

Mesh *gf3d_mesh_get_by_filename(const char *filename){
    int i;
    if (!filename) return NULL;
    for(i = 0; i < mesh_manager.mesh_count; i++){
        if(!mesh_manager.mesh_list[i]._refCount) continue;
        if(gfc_line_cmp(mesh_manager.mesh_list[i].filename, filename) == 0){
            return &mesh_manager.mesh_list[i];
        } //Why are you throwing error??
    }
    /*
    int i;
    if (!filename)return NULL;
    for (i = 0; i < gf2d_sprite.max_sprites; i++)
    {
        if (!gf2d_sprite.sprite_list[i]._inuse)continue;
        if (gfc_line_cmp(gf2d_sprite.sprite_list[i].filename,filename) == 0)
        {
            return &gf2d_sprite.sprite_list[i];
        }
    }
    return NULL; */
    return NULL;
}

Mesh *gf3d_mesh_load(const char *filename){
    MeshPrimitive *prim; 
    Mesh *mesh;
    ObjData *objdata;

    //MESH or MESHprimitive objdata
    // objload??

    mesh = gf3d_mesh_get_by_filename(filename);
    if (mesh)
    {
        mesh->_refCount++; //inuse -> refCount
        return mesh; 
        
        //free if null
        //primiitve is null allocarte arary
        //gfc list append
        //primitiveFromObjc->objdata = objdata
    } // find it by its name, else make a new one 


    objdata = gf3d_obj_load_from_file(filename);
    mesh = gf3d_mesh_new();
    if(!mesh){
        slog("no mesh in mesh load"); 
        free(objdata);
        return NULL;
    }
    prim = gf3d_mesh_primitive_new();
    if(!prim){
        slog("no prim in mesh load"); 
        free(objdata);
        gf3d_mesh_free(mesh);
        return NULL;
    }
    //you forgot to set objdata
    prim->objData = objdata;


    //Per prim basis you need to do the thingy
    //slog("In mesh load b4 create vertex buffer");
    gf3d_mesh_primitive_create_vertex_buffers(prim);
    //slog("In mesh load b4 create face buffer");
    gf3d_mesh_setup_face_buffers(prim, objdata->outFace, objdata->face_count);
    

    gfc_list_append(mesh->primitives, prim);
    return mesh;
    // return mesh;

    // chail length is swapchain gfc_edge_length

    // set the default texture


    //Maybe all this has to go in prim_load?
    /*    
    Sprite *sprite;
    sprite = gf2d_sprite_get_by_filename(filename);
    if (sprite)
    {
        sprite->_inuse++;
        return sprite;
    }
    sprite = gf2d_sprite_new();
    if (!sprite)
    {
        return NULL;
    }
    sprite->texture = gf3d_texture_load(filename);
    if (!sprite->texture)
    {
        slog("gf2d_sprite_load: failed to load texture for sprite");
        gf2d_sprite_free(sprite);
        return NULL;
    }
    sprite->surface = sprite->texture->surface;
    if (frame_width <= 0)frame_width = sprite->texture->width;
    if (frame_height <= 0)frame_height = sprite->texture->height;
    sprite->frameWidth = frame_width;
    sprite->frameHeight = frame_height;
    sprite->widthPercent = sprite->frameWidth / (float)sprite->texture->width;
    sprite->heightPercent = sprite->frameHeight/ (float)sprite->texture->height;
    if (frames_per_line)sprite->framesPerLine = frames_per_line;
    else sprite->framesPerLine = 1;
    gfc_line_cpy(sprite->filename,filename);
    gf2d_sprite_create_vertex_buffer(sprite);
    return sprite; */

}

MeshPrimitive *gf3d_mesh_primitive_new(){ 
    //Its literally one line allocating a new primitive
    // gfc_allocate_array()
    return gfc_allocate_array(sizeof(MeshPrimitive),mesh_manager.mesh_count);
    // Mesh or MeshPrimitive pointer does it return?
    // basically set everything in the primitive to what it nees
    // call the two buffers? face and the other one? 

    /*
    typedef struct
{
    Uint32          vertexCount;
    VkBuffer        vertexBuffer;
    VkDeviceMemory  vertexBufferMemory;
    Uint32          faceCount;
    VkBuffer        faceBuffer;
    VkDeviceMemory  faceBufferMemory;
    ObjData        *objData;
}MeshPrimitive;
    */

    //So the design pattern is that every primitive is treated like one big obj?
    // 
    //gfc_allocate_array();
    //MeshPrimitive *prim;

    //gf3d_mesh_setup_face_buffers(prim,  *faces,  fcount);
    //return prim;
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
        slog("cannot intilizat mesh manager for 0 meshes");
        return;
    }
    mesh_manager.chain_length = gf3d_swapchain_get_chain_length();
    //mesh_manager.mesh_list = (Sprite *)gfc_allocate_array(sizeof(Sprite),mesh_max);
    mesh_manager.mesh_list = (Mesh *)gfc_allocate_array(sizeof(Mesh),mesh_max);
    mesh_manager.mesh_count = mesh_max;
    mesh_manager.device = gf3d_vgraphics_get_default_logical_device();
    
    // setup the face buffer, which will be used for ALL sprites
    /*faces[0].verts[0] = 2;
    faces[0].verts[1] = 1;
    faces[0].verts[2] = 0;
    faces[1].verts[0] = 1;
    faces[1].verts[1] = 3;
    faces[1].verts[2] = 2;

    bufferSize = sizeof(SpriteFace) * 2;
    
    can we set up the face buffers here???
    */
   
    /*gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    vkMapMemory(mesh_manager.device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, faces, (size_t) bufferSize);
    vkUnmapMemory(mesh_manager.device, stagingBufferMemory);
    */
/*   
    slog("Before initalize the mesh manager");
    //bufferSize = sizeof(MeshPrimitive) * 2;
    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mesh_manager.faceBuffer, &mesh_manager.faceBufferMemory);
    
    vkMapMemory(mesh_manager.device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, faces, (size_t) bufferSize);
    vkUnmapMemory(mesh_manager.device, stagingBufferMemory);


    gf3d_buffer_copy(stagingBuffer, mesh_manager.faceBuffer, bufferSize);

    vkDestroyBuffer(mesh_manager.device, stagingBuffer, NULL);
    vkFreeMemory(mesh_manager.device, stagingBufferMemory, NULL);

    Per mesh primitve needs to be allocated
*/
    gf3d_mesh_get_attribute_descriptions(&count);
    slog("Trying to initalize the mesh manager");
    //Its this that gives the mesh manager its pipe
    mesh_manager.pipe = gf3d_pipeline_create_from_config(
        gf3d_vgraphics_get_default_logical_device(),
        "config/overlay_pipeline.cfg",
        gf3d_vgraphics_get_view_extent(),
        mesh_max,
        //mesh_manager._get_bind_;
        gf3d_mesh_get_bind_description(),
        gf3d_mesh_get_attribute_descriptions(&mesh_max),
        count,
        sizeof(MeshUBO),
        VK_INDEX_TYPE_UINT16
    );     

    /*
    VkDevice device,
    const char *configFile,
    VkExtent2D extent,
    Uint32 descriptorCount,
    const VkVertexInputBindingDescription* vertexInputDescription,
    const VkVertexInputAttributeDescription * vertextInputAttributeDescriptions,
    Uint32 vertexAttributeCount,
    VkDeviceSize bufferSize,
    VkIndexType indexType
    */
    
    //if(__DEBUG)slog("sprite manager initiliazed");
    slog("mesh manager initalized");
    atexit(gf3d_mesh_manager_close);
}

void gf3d_mesh_setup_face_buffers(MeshPrimitive *prim, Face *faces, Uint32 fcount){
    //... !!! 
    // Theres a stanging buffer and you copy that into the gpu and then destory it
    void *data = NULL;
    //Face *faces;
    //Uint32 fcount;
    size_t bufferSize;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    faces = prim->objData->outFace;
    fcount = prim->objData->face_count;

    bufferSize = sizeof(Face) * fcount;
    
    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    vkMapMemory(mesh_manager.device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, faces, (size_t) bufferSize);
    vkUnmapMemory(mesh_manager.device, stagingBufferMemory);

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mesh_manager.faceBuffer, &mesh_manager.faceBufferMemory);

    //meshes dont have face buffers??
    //gf3d_buffer_copy(stagingBuffer, mesh->faceBuffer, bufferSize);
    gf3d_buffer_copy(stagingBuffer, mesh_manager.faceBuffer, bufferSize);

    vkDestroyBuffer(mesh_manager.device, stagingBuffer, NULL);
    vkFreeMemory(mesh_manager.device, stagingBufferMemory, NULL);    
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
        vkDestroyBuffer(mesh_manager.device, mesh_manager.faceBuffer, NULL);
    }
    if (mesh_manager.faceBufferMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(mesh_manager.device, mesh_manager.faceBufferMemory, NULL);
    }

    memset(&mesh_manager,0,sizeof(MeshManager));
    if(__DEBUG)slog("sprite manager closed");
    // free the default texture
}

VkVertexInputAttributeDescription * gf3d_mesh_get_attribute_descriptions(Uint32 *count)
{
    mesh_manager.attributeDescriptions[0].binding = 0;
    mesh_manager.attributeDescriptions[0].location = 0;
    mesh_manager.attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    mesh_manager.attributeDescriptions[0].offset = offsetof(Vertex, vertex);

    mesh_manager.attributeDescriptions[1].binding = 0;
    mesh_manager.attributeDescriptions[1].location = 0;
    mesh_manager.attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    mesh_manager.attributeDescriptions[1].offset = offsetof(Vertex, normal);
    
    mesh_manager.attributeDescriptions[2].binding = 0;
    mesh_manager.attributeDescriptions[2].location = 1;
    mesh_manager.attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    mesh_manager.attributeDescriptions[2].offset = offsetof(Vertex, texel);
    if (count)*count = SPRITE_ATTRIBUTE_COUNT; //Were leaving this as sprite because IDK
    return mesh_manager.attributeDescriptions;
}

//primitive_render => primitive_queue_render
void gf3d_mesh_primitive_queue_render(MeshPrimitive *prim,Pipeline *pipe,void *uboData,Texture *texture){

    if((!prim) || (!pipe) || (!uboData)) return;
    //if(!texture) texure = mesh_manager.default //DEFAULT TEXTURE?
    gf3d_pipeline_queue_render(
        pipe,
        prim->vertexBuffer, 
        prim->vertexCount,
        prim->faceBuffer,
        uboData,
        texture);
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
    gfc_color_to_vector4f(mod);

    // gfc_color_copy(&ubo.color, mod);
    // todo color

    //Mesh vs pipeline queue renderer?? 
    //renderer => render
    if(!texture) texture = mesh_manager // load the default texture in the manager not the draw manager.default
    ubo.camera = gfc_vector3dw(gf3d_camera_get_position(), 1.0);//camera get position
    gf3d_mesh_queue_render(mesh, mesh_manager.pipe, &ubo, texture);
        
    //if(!texture) texure = mesh_manager.de //default again
    /*gf3d_pipeline_queue_render(
        pipe
        prim->vertexBuffer, 
        prim
        ..
        uboData
        texture
    )*/
}


void gf3d_mesh_primitive_create_vertex_buffers(MeshPrimitive *prim)
{
    /* facecount, vertcount, face list and face count?, buffersize

        Face buffer vs vertex buffers???? I copy pasted face to here
    */
    void *data = NULL;
    Face *faces;
    Uint32 fcount;
    size_t bufferSize;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    faces = prim->objData->outFace;
    fcount = prim->objData->face_count;

    bufferSize = sizeof(Face) * fcount; //vertex -> face
    // slog("Buffersize %i", bufferSize);
    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);
    vkMapMemory(mesh_manager.device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, faces, (size_t) bufferSize);

    vkUnmapMemory(mesh_manager.device, stagingBufferMemory);
    //VK_BUFFER_USAGE_INDEX_BUFFER_BIT -> VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
    // index for face, vertex for vertex
    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mesh_manager.faceBuffer, &mesh_manager.faceBufferMemory);

    gf3d_buffer_copy(stagingBuffer, mesh_manager.faceBuffer, bufferSize);

    vkDestroyBuffer(mesh_manager.device, stagingBuffer, NULL);
    vkFreeMemory(mesh_manager.device, stagingBufferMemory, NULL);    

    //make sure the primitive has everything it needs
    //prim->vertexCount = vCount; ?
}

Pipeline *gf3d_mesh_get_pipeline(){
    return mesh_manager.pipe;

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

void gf3d_mesh_free(Mesh *mesh)
{
    if (!mesh)return;
    mesh->_refCount--;
    if (mesh->_refCount <= 0)gf3d_mesh_delete(mesh);
    ///This seems wrong
}



/*
    Current great mysteries
    What the hell is the obj data?
    What the hell is SpriteVertex
        its just Vertex
*/