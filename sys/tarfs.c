#include <sys/tarfs.h>
#include <sys/kprintf.h>
#include <sys/defs.h>
#include <sys/str.h>
#include <sys/vfs.h>
#include <sys/vmmu.h>
#include <sys/elf64.h>
//#include 
/*#define O_DIRECTORY = 0x10000 
#define O_WRONLY = 1
#define  O_RDWR = 2
*/

tarfs_entry tarfs_fs[100]; 
char buf[10];

//open, read, close, opendir, readdir, closedir


int get_per_ind(char* dir)
{
    char name[100];
    int len = strlength(dir);
    strcpy(&name[0], dir);
    len = len-2;
    while(name[len] != '/')
    {
        len--;
        if(len == 0)
            return 999;
    } 
    name[++len] = '\0';
    int i = 0;
    while(strcmp(&name[0], &(tarfs_fs[i].name[0])) !=  0)
        i++;
    return i;
}

int get_per_ind_file(char* dir)
{
    char name[100];
    int len = strlength(dir);
    strcpy(&name[0], dir);
    len = len-1;
    while(name[len] != '/')
    {
        len--;
        if(len == 0)
            return 999;
    }
    name[++len] = '\0';
    int i = 0;
    while(strcmp(&name[0], &(tarfs_fs[i].name[0])) !=  0)
        i++;
    return i;
}


uint64_t octal_to_int(char *optr, int length) {
    uint64_t val = 0;
    if(optr == NULL)
        return 0;
    while(length-- > 0 && *optr >= '0' && *optr <= '7') {
        val <<= 3;
        val += *optr++ - '0';
    }
    return val;
}



unsigned int getSize(const char *in)
{
 
    unsigned int size = 0;
    unsigned int j;
    unsigned int count = 1;
 
    for (j = 11; j > 0; j--, count *= 8)
        size += ((in[j - 1] - '0') * count);
 
    return size;
 
}
uint64_t getElfHeader(char * fileName)
{
	uint64_t address= (uint64_t)&_binary_tarfs_start;
        unsigned int i;
        for(i =0 ; address <(uint64_t)&_binary_tarfs_end ;i++){
                struct posix_header_ustar *header = (struct posix_header_ustar*)address;
                unsigned int size = getSize(header->size);
		if(strcmp(header->name,fileName)==0)
		{
			return (uint64_t) (header+1);
		}
                address += ((size/512) +1) * 512;
                if(size %512)
                        address+=512;
        }
return (uint64_t)NULL;
}
/*
void read_elf(uint64_t header)
{
	Elf64_Ehdr *elfhdr = (Elf64_Ehdr *) (header);
 kprintf("\nentry %d", elfhdr->e_phoff);    
                   // uint64_t addr=header;

                        if(elfhdr->e_phoff>0)
                        {
                                addr += elfhdr->e_phoff;
				Elf64_Phdr *ephdr = (Elf64_Phdr *)(addr);
				if(ephdr!=NULL)
					kprintf("Summa\n");
                        kprintf("\nentry %p", ephdr->p_type);
//                        kprintf("\nentry %x", elfhdr2->e_type);                        
}
                      //  Elf64_Ehdr *elfhdr2 = (Elf64_Ehdr *)(addr);
                      //  kprintf("\nentry %x", elfhdr2->e_entry);
                      //  kprintf("\nentry %x", elfhdr2->e_type);
}
int find_elf_header(char *path)
{
uint64_t address= (uint64_t)&_binary_tarfs_start; 
struct posix_header_ustar *header = (struct posix_header_ustar*)address;        
unsigned int i=0;
while(address <(uint64_t)&_binary_tarfs_end){
                unsigned int size = getSize(header->size);
                headers[i++] = header;
                //tarfs_open(header,O_RDONLY,0,0);             
//              Elf64_Ehdr *elfhdr = (Elf64_Ehdr *) (header+1);
//              uint64_t addr;
//              if(elfhdr->phoff > 0)
                {
//                      addr = elfhdr + elfhdr->phoff;
                }
//              kprintf("\nentry %x", elfhdr->e_entry);
//              kprintf("\inentry %x", elfhdr->e_type);
                //kprintf("\nphysoffset %x num %d", elfhdr->e_phoff, elfhdr->e_phnum);
                //kprintf("\nshoffset %x num %d", elfhdr->e_shoff, elfhdr->e_shnum);
  //              kprintf("\nhname: %s, %d", header->name, header->size);
                if(strcmp(path,header->name)==0)
		{
			read_elf((uint64_t)(header+1));
		}
		address += ((size/512) +1) * 512;
                if(size %512)
                        address+=512;
		header = (struct posix_header_ustar *) address;
        }
return 0;
}
//initializing tarfs
void init_tarfs(){
	uint64_t address= (uint64_t)&_binary_tarfs_start;
	unsigned int i;
        //int temp =0;
        tarfs_entry tarfs_e;
	for(i =0 ; address <(uint64_t)&_binary_tarfs_end ;i++){
		struct posix_header_ustar *header = (struct posix_header_ustar*)address;
		unsigned int size = getSize(header->size);


		headers[i] = header;
                strcpy(&tarfs_e.name[0], header->name);
                tarfs_e.size = size;
                tarfs_e.addr_hdr = address;                       
                tarfs_e.typeflag = header->typeflag[0];

                if(tarfs_e.typeflag == 1)
                tarfs_e.par_ind = get_per_ind_file(&(tarfs_e.name[0]));
                else if(tarfs_e.typeflag == 5)
                 {
                tarfs_e.par_ind = get_per_ind(&(tarfs_e.name[0]));
                 }                
                tarfs_fs[i] = tarfs_e;
                if(size>0)		
		{	read_elf((uint64_t)(header+1));

                }


        	tarfs_open(header,O_RDONLY,0,0);
                uint64_t a = open_dir(header->name);
                kprintf("open dir , %x",a);             
        	//kprintf("\nhname: %s, %d", header->name, header->size);
		address += ((size/512) +1) * 512;
		if(size %512)
			address+=512;
		
	}
}


static struct posix_header_ustar *tarfs_next(struct posix_header_ustar *address) 
{

        struct posix_header_ustar *header = (struct posix_header_ustar*)address;
        unsigned int size = getSize(header->size);
        address += ((size/512) +1) * 512;
        if(size %512)
            {
             address+=512;
            }
        header = address;

        return header;
   

        if(!cur_hdr || cur_hdr->name[0] == '\0')
        {
        
             return NULL;
        
        }
        else
        { 
        
        uint64_t size = octal_to_int(cur_hdr->size, sizeof(cur_hdr->size));
        cur_hdr  += 1 + size/512 + (size % 512 != 0);
        if(cur_hdr->name[0] == '\0')
            return NULL;
        else
            return cur_hdr;
        }







}
struct file * tarfs_open(struct posix_header_ustar *tarfheader,int flags,uint64_t mode, int *err)
{
    struct file* fp = kmalloc(sizeof(struct file));
    if(!fp) 
    {
        return NULL;
    }
    fp->private_data = tarfheader;
    fp->f_size = octal_to_int(tarfheader->size, sizeof(tarfheader->size));
    if(tarfheader->typeflag[1] == TARFS_DIRECTORY)
        fp->f_pos = (uint64_t)tarfs_next(tarfheader);
    else
        fp->f_pos = 0;
    fp->f_error = 0;
    fp->f_flags = flags;
    fp->f_count = 1;
    //kprintf("Inside tarfs open,%x",fp);
    tarfs_read(fp,buf,sizeof(buf) - 1,&fp->f_pos);
    return fp;
}

uint64_t  tarfs_read(struct file *fp, char *buf, size_t count, off_t *offset)
{

struct posix_header_ustar *tarfheader = (struct posix_header_ustar *)fp->private_data;
char *file_data_start;
size_t bytes_left, num_read;

if(count == 0 ||  *offset == fp->f_size)
{
  kprintf("size %d",fp->f_size);
  return 0;
}

bytes_left = fp->f_size - *offset;

if(bytes_left<count)
{
num_read = bytes_left;
}
else
{
num_read = count;
}

file_data_start = (char *)(tarfheader + 1);
memcpy(buf, file_data_start + *offset, num_read);
*offset += num_read;
kprintf("Number of bytes read,%d %s", num_read,buf); 
return num_read;

}


uint64_t open_dir(char * dir)
{
        if(strlength(header->name) == 0)
            return 0;
        if(strcmp(header->name,dir) == 0 && header->typeflag[0] == 5)
            return header;

    return 0;
}


uint64_t read_dir(uint64_t addr)
{
    tarfs_entry tarfs_e;
    char *dir;
    int i = 0, parent = -1, len = 0;
    if(addr == 999)
        parent = 999; 
    while(1)
    {
        tarfs_e = tarfs_fs[i];
        if(strlength(tarfs_e.name) == 0)
            break;

        if( tarfs_e.addr_hdr == addr)
        {  
            parent = i;
            i++;
            dir = tarfs_e.name;
            len = strlength(dir);
            continue; 
        }
        if((strcmp(&(tarfs_e.name[0]), dir) &&( tarfs_e.par_ind == parent)))
              {  
                  kprintf("%s", tarfs_e.name + len);
              }
        i++;
    }
    return 0;
}
*/

 
