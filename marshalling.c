RequestData marshalling(int direction[]){
	RequestData * dataS = malloc(sizeof(RequestData));
	dataS->data = "IZQUIERDA";
	dataS->size = sizeof(dataS->data);
	return *dataS;
}

char * unmarshalling(ResponseData * data){
	char * dataR=malloc(2*sizeof(char));
	if(1){
		//se pregunta para ver que es lo que recibe y modificar dataR
	}
	return dataR;
}