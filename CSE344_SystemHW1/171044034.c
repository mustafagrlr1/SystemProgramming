#include <stdio.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


//Returns length of string
size_t strlen(const char* string){
	size_t n = 0;

	while(string[n]) ++n;

	return n;
}


//Returns how many operations in argument 1
int count_semi_colon(const char* argv){

	int count = 0;
	
	for(int i=0 ; i < strlen(argv) ; i++){
		if(argv[i] == ';'){
			count++;
		}
	}
	
	count++;

	return count;

}


//Returns single operation splitted by ';'
char** parse_argument(const char* argv, int count){

	
	int place;
	int i = 0;
	int parsed_string_it = 0;
	int j=0;

	char** parsed_string = malloc(count * sizeof(char));
	

	for(i=0 ; i < strlen(argv)+1 ; i++){
		if(argv[i] == ';'){
			place = i-j;
			char* string = malloc(place * sizeof(char));
			for(int x=0 ; x<place ; x++){
				string[x] = argv[j];
				j++;
			}
			parsed_string[parsed_string_it] = string;
			parsed_string_it++;
			j++;
		}
	}

	place = i-j;
	char*string = malloc(place * sizeof(char));
	for(int x=0 ; x<place ; x++){
		string[x] = argv[j];
		j++;
	}
	parsed_string[parsed_string_it++] = string;



	return parsed_string;

}

//Returns single operations strings splitted by '/'
char** read_until_slash(char** parsed_string, int process_number){

	int place;
	int i=1;
	int j=1;
	int count_process_step = 0;
	char** process_string = malloc(3 * sizeof(char));

	for(i=1 ; i < strlen(parsed_string[process_number]) ; i++ ){
		if(parsed_string[process_number][i] == '/'){
			place = i-j;
			char* string = malloc(place*sizeof(char));
			for(int x=0 ; x<place ; x++){
				string[x] = parsed_string[process_number][j];
				j++;
			}
			process_string[count_process_step] = string;
			count_process_step++;
			j++;
		}

	}

	place = i-j;
	if(parsed_string[process_number][i-1] == 'i'){
		char* string = malloc(sizeof(char));
		string[0] = 'i';
		process_string[count_process_step] = string;
		
	}else{
		char* string = malloc(sizeof(char));
		string[0] = 'b';
		process_string[count_process_step] = string;
		
	}

	return process_string;
}

int consist_number(unsigned char* buffer, int it){

	for(int i=0 ; i<10 ; i++){
		if(buffer[it] == 48+i){
			return 1;
		}
	}
	return 0;
}

int consist_of_paranthesis(char* buffer, int it){
	
	if(buffer[it] == '['){
		
		do{
			it++;		
		}while(buffer[it] != ']');

	}
	return it;
		

}

int consist_of_paranthesis_find(char*string){
	int start=0; int end=0;
	for(int i=0 ; i < strlen(string);i++){
		if(string[i] == '['){
				start=1;
		}
		if(string[i] == ']'){
			end = 1;
		}

	}

	if(start == 1 && end == 1){
		return 1;
	}
	return -1;

}
int consist_number_of_paranthesis_start(char* string){
	
	for(int i=0 ; i < strlen(string);i++){
		if(string[i] == '['){
			return i;
		}
	}
	return -1;
}
int consist_number_of_paranthesis_end(char* string){
	
	for(int i=0 ; i < strlen(string); i++){
		if(string[i] == ']'){
			return i;
		}
	}
	return -1;
}
int consist_of_asterix_for_string(char* string){
	for(int i=0 ; i<strlen(string) ; i++){
		if(string[i] == '*'){
			return i;
		}
	}
	return -1;
}


int find_of_asterix_number(unsigned char* buffer, int it, char* string, int value){
	
		
	int j=0;
	while(buffer[it+j] == string[value-1]){
		j++;
	}
		
	return j;
}

//yildiz varsa bufferda ki yildizli olmasi gereken harflerin basini bulurum.
int find_of_asterix(unsigned char* buffer, int it, char* string, int value){


	
	for(int i=0 ; i<value ; i++){
		if(buffer[it] != string[value-1]){
			break;	
		}else{
			for(int j=i ; j<value ; j++){
				if(buffer[it+j] != string[value-1]){
					break;
				}
				if(j==value-1){
					return i;
				}
			}
		}
	}
	
	return -1;

}

int find_of_began_line_string(char* string){
	for(int i=0 ; i<strlen(string) ; i++){
		if(string[i] == '^'){
			return i;
		}
	}
	return -1;
}

int find_of_end_line_string(char* string){
	for(int i=0 ; i<strlen(string) ; i++){
		if(string[i] == '$'){
			return i;
		}
	}
	return -1;
}



//Iterated buffer checks characters with parsed string iterated by string_it insensitive means we have case insenstive characters
int part_a(unsigned char* buffer,int it, char* parsed_string, int length,int string_it, int insensitive, int asterix_flag, int asterix_number, char* last_string, int enter){
	int value = -1;
	
	value = consist_of_asterix_for_string(parsed_string);
	if(length-1 == string_it  && enter == 2 ){ // checks if the $ sign is inside of the parsed string 
		if(buffer[it+1] == 10 || buffer[it+1] == 13){ 
			length = length -1;
		}
	}

	// checks asterix and find asterix number
	if(asterix_flag == 1 && value-1 == string_it && buffer[it] == parsed_string[string_it]){
		
		asterix_number = find_of_asterix_number(buffer, it, parsed_string, value);
		string_it = string_it+2;
		it = it + asterix_number;
		asterix_flag = 0;
	}
	// if they are equal we have a match
	if(string_it == length){
		if(consist_of_asterix_for_string(parsed_string) >= 0){
			return asterix_number;
		}
		return 1;
	}
	else if(consist_number(buffer, it)){ // keeps searching 
		if(buffer[it] == parsed_string[string_it]){	
			return part_a(buffer, it+1 , parsed_string, length, string_it+1, insensitive, asterix_flag, asterix_number, last_string, enter) ;
		}
		else if(insensitive == 1 && (buffer[it] == parsed_string[string_it] || buffer[it] == parsed_string[string_it] -32 || buffer[it] == parsed_string[string_it] + 32)){
			
			return part_a(buffer, it+1 , parsed_string, length, string_it+1, insensitive, asterix_flag, asterix_number, last_string, enter) ;
		}
		else if(buffer[it] == parsed_string[string_it]){
		
			return part_a(buffer, it+1 , parsed_string, length, string_it+1, insensitive, asterix_flag, asterix_number, last_string, enter) ;
		}else{
			return 0;
		}
	}
	else if(insensitive == 1 && (buffer[it] == parsed_string[string_it] || buffer[it] == parsed_string[string_it] -32 || buffer[it] == parsed_string[string_it] + 32)){
		
		return part_a(buffer, it+1 , parsed_string, length, string_it+1, insensitive, asterix_flag, asterix_number, last_string, enter) ;
	}
	else if(buffer[it] == parsed_string[string_it]){
		return part_a(buffer, it+1 , parsed_string, length, string_it+1, insensitive, asterix_flag, asterix_number, last_string, enter) ;

	}else{
		return 0;
	}

}
//start of the searching, finds first character matches
int part_a_start(unsigned char* buffer,int it, char* parsed_string, int string_it,char* last_string, int enter){

	int part_a_result1 = 0;
	if(enter == 1){
		string_it++;
	}
	
	int length = strlen(parsed_string);

	int asterix_flag = -1;
	int value = -1;
	int asterix_number = -1;
	//checks if there is an asterix
	if(consist_of_asterix_for_string(parsed_string) > 0){
		asterix_flag = 1;
		value = consist_of_asterix_for_string(parsed_string);
		
	}

	if(value-1 == string_it && buffer[it] == parsed_string[string_it]){
		asterix_number = find_of_asterix_number(buffer, it, parsed_string, value);
	
		string_it = value+1;
		it = it + asterix_number;
	}
	//checks if there is case insensiteve characters
	if(last_string[0] == 'i'){
		if(consist_number(buffer, it) == 1){
			if(buffer[it] == parsed_string[string_it]){
				part_a_result1 = part_a(buffer, it+1, parsed_string, length, string_it+1, 1, asterix_flag, asterix_number, last_string, enter);	
			}		
		}else{
			if(buffer[it] == parsed_string[string_it] || buffer[it] == parsed_string[string_it] - 32 || buffer[it] == parsed_string[string_it] + 32){
				part_a_result1 = part_a(buffer, it+1, parsed_string, length, string_it+1, 1, asterix_flag, asterix_number, last_string, enter);	
			}	
		}
		
	}else{
		
		if(buffer[it] == parsed_string[string_it]){

			part_a_result1 = part_a(buffer, it+1, parsed_string, length, string_it+1, 0, asterix_flag, asterix_number, last_string, enter);	
			
		}	
	}
	

	// if we have a match, returns a number that bigger than 0
	if(part_a_result1 != 0){
		
		if(asterix_flag == 1){
			return part_a_result1+1;
		}
		return part_a_result1;
			
	}
	return 0;

	

}



int read_from_fd(char* fileName, char** parsed_string){
	
	unsigned char buffer[1000];
	size_t bytes_read;
	int i;
	int part_a_result;
	int fd = open(fileName, O_RDWR, S_IRWXU );

	if(fd == -1){
		perror("error");
		return 1;
	}

	//lock file
	struct flock lock;

	memset(&lock, 0, sizeof(lock));
	lock.l_type = F_WRLCK;

	if(fcntl(fd, F_SETLKW, &lock)){
		perror("error");
		return 1;
	}



	char array[1000000];
	int count =0;
	int start_number_paranthesis = 0;
	int end_number_paranthesis = 0;
	

	int paranthes_length = 0;
	
	int enter = 0 ;
	do{ 
		bytes_read = read(fd, buffer, sizeof(buffer));

		for(i=0 ; i<bytes_read ;){
			enter=0;
			//find ^ character 
			if(find_of_began_line_string(parsed_string[0]) >= 0){

				if(buffer[i-1] == 10 || buffer[i-1] == 13 || i == 0){
					enter = 1;
				}
			}


			// if(find_of_end_line_string(parsed_string[0]) >= 0 || i == bytes_read){
			// 	enter = 2;
			// }
			int s=0;

			//if there are paranthesis they need to be handled different 
			if (consist_of_paranthesis_find(parsed_string[0]) == 1){
				start_number_paranthesis = consist_number_of_paranthesis_start(parsed_string[0])+1;
				end_number_paranthesis = consist_number_of_paranthesis_end(parsed_string[0])-1;
				char temp_array[100];
				
				int temp_array_it = 0;
				
				int value = end_number_paranthesis - start_number_paranthesis + 1;

				for(int j=0 ; j<value ; j++){
					s=0;
					temp_array_it = 0;

					for(int k=0 ; k<start_number_paranthesis-1 ; k++){
						temp_array[temp_array_it] = parsed_string[0][k];
						temp_array_it++; 
					}

					temp_array[temp_array_it] = parsed_string[0][start_number_paranthesis+j];
					temp_array_it++;

					for(int k=end_number_paranthesis+2 ; k<strlen(parsed_string[0]) ; k++){

						temp_array[temp_array_it] = parsed_string[0][k];
						temp_array_it++;
					} 
							
					part_a_result = part_a_start(buffer, i, temp_array, 0, parsed_string[2], enter);
					if(part_a_result > 0){
						paranthes_length = value+1;
						break;
					}
				}
				if(part_a_result > 0){
					
					if(part_a_result == 1){
						if(enter == 1 || enter == 2){
							i = i - 1;
						}
						i += strlen(parsed_string[0]) - paranthes_length ;
					}else{
						if(part_a_result > 1){
							if(enter == 1 || enter == 2){
								i = i - 1;
							}
							i += strlen(parsed_string[0]) + part_a_result - 3 - paranthes_length;	
						}
					}
					for(int q = count; q<count+strlen(parsed_string[1]);q++){
						array[q] = parsed_string[1][s];
						s++;
					}
					count += s;
					
					
				}else{
					array[count] = buffer[i];
					count++;
					i++;
				}
			}else{
				part_a_result = part_a_start(buffer, i, parsed_string[0], 0, parsed_string[2], enter);
				s=0;
				if(part_a_result > 0){	
					if(part_a_result == 1){
						if(enter == 1 || enter == 2){
							i = i - 1;
						}
						i += strlen(parsed_string[0]) - paranthes_length ;
					}else{
						if(enter == 1 || enter == 2){
							i = i - 1;
						}
						i += strlen(parsed_string[0]) + part_a_result - 3 - paranthes_length;	
					}
					for(int q = count; q<count+strlen(parsed_string[1]);q++){
						array[q] = parsed_string[1][s];
						s++;
					}
					
					count += s;
					
				}else{
					array[count] = buffer[i];
					count++;
					i++;
				}	
			}

		}		
	 }while(bytes_read == sizeof(buffer));


	lock.l_type = F_UNLCK;

	if(fcntl(fd, F_SETLKW, &lock) == -1){
		perror("error");
		return 1;
	}

	if(close(fd) == -1){
		perror("error");
		return 1;
	}

	fd = open(fileName, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU );



	if(fd == -1){
		perror("error");
		return 1;
	}
	
	memset(&lock, 0, sizeof(lock));
	lock.l_type = F_WRLCK;

	if(fcntl(fd, F_SETLKW, &lock) == -1){
		perror("error");
		return 1;
	}

	if(write(fd, array, count) == -1){
		perror("error");
		return 1;
	}

	lock.l_type = F_UNLCK;

	if(fcntl(fd, F_SETLKW, &lock)){
		perror("error");
		return 1;
	}

	if(close(fd) == -1){
		perror("error");
		return 1;
	}	
		
	return 1;
}


int replace_all_occurences(char* fileName, char* string){
	

	int count = count_semi_colon(string);
	
	char** parsed_string = parse_argument(string, count);


	char** process_string;

	for(int i=0 ; i<count ; i++){
		process_string = read_until_slash(parsed_string, i);
		read_from_fd(fileName, process_string);	
		for(int i=0 ; i<3 ; i++){
			free(process_string[i]);
		}

	}
	free(process_string);

	for(int i=0 ; i<count ; i++){
		free(parsed_string[i]);
	}
	free(parsed_string);
	
	return 1;
}


int main(int argc, char* argv[]){
	if(argc != 3){
		perror("invalid arguments");
		return 1;
	}

	replace_all_occurences(argv[2], argv[1]);

	return 0;



}
