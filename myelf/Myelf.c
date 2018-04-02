#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include</usr/include/elf.h>
#include<fcntl.h>// include read open close function  
#define SIZE  128

//print head information
int Elfhead(Elf32_Ehdr* elf )
{
	printf("ELF Header:\n  Magic:");//           Magic
	int i;
	for (i=0; i<16; i++)
	{
		printf("%02x ",elf->e_ident[i]);
	}
	printf("\n");

    if(elf->e_ident[5] == 1)//                  Class
	{
	    printf("  Class:         ELF32\n");
	}
	else if(elf->e_ident[5] == 2)
	{
		printf("  Class:         ELF64\n");
	}
	else
	{
		printf("  Class:          invalid file\n");
	}

    if(elf->e_ident[6] == 1)//                  Data
	{
		printf("  Data:         2`s complement, little endian\n");
	}
	else if(elf->e_ident[5] == 2)
	{
		printf("  Data:         2`s complement, big endian\n");
	}
	else
	{
		printf("  Data:         invalid\n");
	}
    printf("  Version:          1 (current)\n");//     version
	printf("  OS/ABI:           NUIX - System V\n");
	printf("  ABI Version:      0\n");
	if (elf->e_type == 1)//                 type
	{
	  printf("  Type:             REL (Relocatable file)\n");
	}
	else if (elf->e_type == 2)
	{
	  printf("  Type:             EXEC (Executable file)\n");
	}
	else
	{

	  printf("  Type:             DYN (Share target file)\n");
	}
	switch (elf->e_machine)//             machine
	{
		case 1:
             printf("  Machine:          AT&T WE 32100\n");
		     break;
		case 2:
             printf("  Machine:          SPARC\n");
		     break;
		case 3:
             printf("  Machine:          Intel x86\n");
		     break;
		case 4:
             printf("  Machine:          Motorola 68000\n");
		     break;
		case 5:
             printf("  Machine:          Motorola 88000\n");
		     break;
		default:
             printf("  Machine:          Intel 80860\n");
		     break;
	}
	printf("  Version:          0x%x\n",elf->e_version);
	printf("  Entry point address: 0x%x\n",elf->e_entry);
	printf("  Start of program headers: %x (bytes into file)\n",elf->e_phoff);
	printf("  Start of section headers: %x (bytes into file)\n",elf->e_shoff);
	printf("  Flags:                    0x%x\n",elf->e_flags);
	printf("  Size of this header:      %d (bytes)\n",elf->e_ehsize);
	printf("  Size of program headers:   %d (bytes)\n",elf->e_phentsize);
	printf("  Number of program headers: %d\n",elf->e_phnum);
	printf("  Size of section headers:   %d (bytes)\n",elf->e_shentsize);
	printf("  Number of section headers:  %d\n",elf->e_shnum);
	printf("  Section header string table index: %d\n",elf->e_shstrndx);
	exit(0);
}

//find name return section size;off return str section start address
int Name(char*argv[],int *off,const char*str)
{
	Elf32_Ehdr head;

	int fd = open(argv[2],O_RDONLY);
	if (fd == -1)
	{
		perror(NULL);
		exit(0);
	}
    int m = read(fd,&head,sizeof(Elf32_Ehdr));
	if (m == 0)
	{
		perror(NULL);
		exit(0);
	}

	lseek(fd,head.e_shoff,SEEK_SET);//move pointer to find section address
	Elf32_Shdr section[head.e_shnum];//section`s arrry
	int n = read(fd,&section,sizeof(Elf32_Shdr)*head.e_shnum);//read all section information
	if (n == 0)
	{
		perror(NULL);
		exit(0);
	}
	int i;
	char name[SIZE];
	for(i=0; i<head.e_shnum; i++)
	{
	    lseek(fd,section[head.e_shstrndx].sh_offset+section[i].sh_name,SEEK_SET);
	//	char name[SIZE];//                                      name
		n = read(fd,name,sizeof(name));
		if(n == 0)
		{
			name[0] = '\0';
		}
		if(strcmp(name,str) == 0)//.symtab
		{
		//	printf("%s   %d\n",name,section[i].sh_size);
			*off = section[i].sh_offset;
			return section[i].sh_size;
		}
	}

}

//printf section table information
int Elfsection(char*argv[])  
{
	Elf32_Ehdr head;

	int fd = open(argv[2],O_RDONLY);
	if (fd == -1)
	{
		perror(NULL);
		exit(0);
	}
    int m = read(fd,&head,sizeof(Elf32_Ehdr));
	if (m == 0)
	{
		perror(NULL);
		exit(0);
	}

	lseek(fd,head.e_shoff,SEEK_SET);//move pointer to find section address
	Elf32_Shdr section[head.e_shnum];//section`s arrry
	int n = read(fd,&section,sizeof(Elf32_Shdr)*head.e_shnum);//read all section information
	if (n == 0)
	{
		perror(NULL);
		exit(0);
	}

	 printf("There are %d section headers, starting at offset ox%x:\n\n",head.e_shnum,head.e_shoff);
	printf("Section Headers:\n");
	printf("%6s %-18s %-10s %-9s %-7s %-7s %-3s %-5s %-3s %-5s %-3s\n",
			"[Nr]","Name","Type","Addr","off","size","ES","Flag","LK","Inf","Al");

	int i;
	for(i=0; i<head.e_shnum; i++)
	{
	//	lseek(fd, head.e_shoff+head.e_shstrndx*40+16,SEEK_SET);
	    lseek(fd,section[head.e_shstrndx].sh_offset+section[i].sh_name,SEEK_SET);
		char name[SIZE];//                                      name
		n = read(fd,name,sizeof(name));
		if(n == 0)
		{
			name[0] = '\0';
		}

        char *mytype;
		switch(section[i].sh_type)//                             mytype
		{
			case 0:
				mytype = "NULL";
				break;
			case 1:
				mytype = "PROGBITS";
				break;
			case 2:
				mytype = "SYMTAB";
				break;
			case 3:
				mytype = "STRTAB";
				break;
			case 4:
				mytype= "RELA";
				break;
			case 5:
				mytype= "HASH";
				break;
			case 6:
				mytype= "DYNAMIC";
				break;
			case 7:
				mytype = "NOTE";
				break;
			case 8:
				mytype= "NOBITS";
				break;
			case 9:
				mytype = "REL";
				break;
			case 10:
				mytype = "SHLIB";
				break;
			default:
				mytype = "DNYSYM";
				break;
		}

		char *flag;
		switch(section[i].sh_flags)
		{
			case 2:
				flag = "A";
				break;
			case 3:
				flag = "WA";
				break;
			case 6:
				flag = "AX";
				break;
			default:
				flag = " ";//????NULL and 0
				break;
		}

        printf("[%2d] %-18s %-10s %-.8x %-.6x %-06x %-02x %-5s %-3x %-5x %-3x\n",i,name,mytype,section[i].sh_addr,section[i].sh_offset,section[i].sh_size,section[i].sh_entsize,flag,section[i].sh_link,section[i].sh_info,section[i].sh_addralign);
	    
	}
	close(fd);
}


//print symbol table information
void SymTab(char*argv[])
{
	Elf32_Ehdr head;//file head`s struct 
	Elf32_Shdr section[SIZE];//section table array
	Elf32_Sym syt[SIZE];//symbol table`s array
	int fd = open(argv[2],O_RDONLY);
    if(fd == -1)
	{
		perror("");
		exit(0);
	}

	int n = read(fd,&head,sizeof(head));
	if(n == 0)
	{
		perror(NULL);
		exit(0);
	}
	lseek(fd,head.e_shoff,SEEK_SET);//move to section table
	n = read(fd,&section,sizeof(head.e_shentsize*head.e_shnum));
	if (n == 0)
	{
		perror(NULL);
		exit(0);
	}

	int add = 0;
	int num = Name(argv,&add,".symtab")/sizeof(Elf32_Sym); 
	printf("Symbol table '.symtab' contains %d  entries:\n",num);
	printf("Num:  Value     Size     Type   Bind   Vis      Ndx       Name\n");

	int strtabadd = 0;//the start address of .strtab
	int strtabsize = Name(argv,&strtabadd,".strtab");
	
	lseek(fd,strtabadd,SEEK_SET);//move pointer to .strtab
	char syname[strtabsize];
	n = read(fd,&syname,sizeof(syname));
	if(n == 0)
	{
		perror(NULL);
		exit(0);
	}
//printf("%d,%d\n",strtabsize,n);

	lseek(fd,add,SEEK_SET);
	n = read(fd,&syt,num*sizeof(Elf32_Sym));
	int i;
	for(i=0; i<num; i++)
	{
		char *str = NULL;
		int sytype = syt[i].st_info % 16;//get symbol type(low 4 bit)
		switch(sytype)
		{
			case 0:
				str = "NOTYPE";
				break;
			case 1:
				str = "OBJECT";
				break;
			case 2:
				str = "FUNC";
				break;
			case 3:
				str = "SECTION";
				break;
	    	default:
				str = "FILE ";
				break;
		}
        int bind = syt[i].st_info >> 4;//high 4bit,unsigned char add 0
        char *str1 = NULL;
		switch(bind)
		{
			case 0:
				str1 = "LOCAL";
				break;
			case 1:
				str1 = "GLOBAL";
				break;
			case 2:
				str1 = "WEAK";
				break;
			default:
				str1 = " ";
				break;
		}

        printf("%2d: %.8x %4.1d",i,syt[i].st_value,syt[i].st_size);
		printf("%12s %7s DEFAULT",str,str1);
    
		if(syt[i].st_shndx == 0xfff1)
		{
			printf("   ABS  ");
		}
		else if(syt[i].st_shndx == 0xfff2)
		{
			printf("   COM  ");
		}
		else if (syt[i].st_shndx == 0)
		{
			printf("   NUD  ");
		}
		else
		{
			printf("   %-4x  ",syt[i].st_shndx);
		}

		int j = syt[i].st_name;
	    while(syname[j] != '\0')
		{
			printf("%c",syname[j]);
			j++;
		}
		printf("\n");
	}

    close(fd);
}

int main(int argc,char* argv[])
{
	
	if (argc < 2)
	{
		printf("error\n");
		exit(0);
	}

	Elf32_Ehdr head;
	int fd;
	fd = open(argv[2],O_RDONLY);//命令、参数、文件名,readonly
	if(fd == -1)
	{
		printf("error\n");
		exit(0);
	}
	read(fd,&head,sizeof(Elf32_Ehdr));   //读取文件

	if(strcmp(argv[1],"-h") == 0)//head
	{
	    Elfhead(&head);
		exit(0);
	}

	if(strcmp(argv[1],"-S") == 0)//section
	{
	   	Elfsection(argv);//?
		exit(0);
	}
	
	if(strcmp(argv[1],"-s") == 0)
	{
		SymTab(argv);
		exit(0);
	}

    close(fd);
    exit(0);
}
