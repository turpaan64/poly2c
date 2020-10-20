#ifndef _TYPES_H_INCLUDED_
#define _TYPES_H_INCLUDED_

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} vec3s_t;

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} z64_bgcheck_vertex_t;

typedef struct
{
    uint16_t info;
    uint16_t v[3];
    int16_t a, b, c, d;
} z64_bgcheck_polygon_t;

typedef struct
{
    uint32_t info[2];
} z64_bgcheck_polygon_info_t;

typedef struct
{
    uint16_t type;
    uint16_t num;
    vec3s_t *table;
} z64_bgcheck_camera_data_t;

typedef struct
{
    vec3s_t min_pos;
    int16_t size_x, size_y;
    uint32_t info;
} z64_bgcheck_water_info_t;

typedef struct
{
    int16_t vtx_min[3];
    int16_t vtx_max[3];
    uint16_t vtx_num;
    uint16_t pad;
    uint32_t vtx_table; /* z64_bgcheck_vertex_t* */
    uint16_t poly_num;
    uint16_t pad2;
    uint32_t poly_table;        /* z64_bgcheck_polygon_t* */
    uint32_t poly_info_table;   /* z64_bgcheck_polygon_info_t* */
    uint32_t camera_data_table; /* z64_bgcheck_camera_data_t*  */
    uint16_t water_info_num;
    uint32_t water_info_table; /* z64_bgcheck_water_info_t* */
} z64_bgcheck_data_info_t;

#endif