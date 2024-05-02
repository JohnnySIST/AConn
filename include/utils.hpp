#ifndef ACONN__UTILS__
#define ACONN__UTILS__

int my_gets(char* str,int num){
	int cnt=0;
	char x=getchar();
	while(x!='\n'&&cnt<num-1){
		str[cnt++]=x;
		x=getchar();
	}
	str[cnt++]='\0';
	return cnt;
}

#endif // ACONN__UTILS__