// CROOKED

// if (argc < 2)
// {
//     print_usage();
//     return 1;
// }

// /* Load the binary as BIG ENDIAN*/
// int zobjFileSize = 0;
// unsigned char *zobj = makeFileBuffer(argv[1], ENDIAN_BIG, &zobjFileSize);

// /* Copy data to the properly modified struct*/
// z64_bgcheck_data_info_t *t;
// memcpy(t, zobj, sizeof(z64_bgcheck_data_info_t));

// /* Print out the formatted data */
// printf("z64_bgcheck_data_info_t %s = {\n", "bgcheck_data");
// printf("\t.vtx_min = {%d, %d, %d}\n", t->vtx_min[0], t->vtx_min[1], t->vtx_min[2]);
// printf("\t, .vtx_max = {%d, %d, %d}\n", t->vtx_max[0], t->vtx_max[1], t->vtx_max[2]);
// printf("\t, .vtx_num = %d\n", t->vtx_num);
// printf("\t, .vtx_table = 0x%08X\n", SWAP_LE_BE(SWAP_V64_BE(t->vtx_table)));
// printf("\t, .poly_num = %d\n", t->poly_num);
// printf("\t, .poly_table = 0x%08X\n", SWAP_LE_BE(SWAP_V64_BE(t->poly_table)));
// printf("\t, .poly_info_table = 0x%08X\n", SWAP_LE_BE(SWAP_V64_BE(t->poly_info_table)));
// printf("\t, .camera_data_table = 0x%08X\n", SWAP_LE_BE(SWAP_V64_BE(t->camera_data_table)));
// printf("\t, .water_info_num = %d\n", t->water_info_num);
// printf("\t, .water_info_table = 0x%08X\n", SWAP_LE_BE(SWAP_V64_BE(t->water_info_table)));
// printf("};\n");

// return 0;