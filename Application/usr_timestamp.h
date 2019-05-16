//This data structure is used by the parser, to store the data received by the device after parsing
typedef struct{
	int usr_data_count;     //length of Token_id + Token string
	char* usr_data;         //Token_id + Token string
	unsigned char a_ts[3];  //Arrival time stamp
	unsigned char d_ts[3];  //Departure time stamp
}U_TS_Data, *U_TS_Data_t;
