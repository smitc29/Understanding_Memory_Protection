#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include<unistd.h>
#include <limits.h>
#include <signal.h>
#include <malloc.h>
#include<string.h>

/*Error Handling*/
 #define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

       static char *buffer;

       static void
       handler(int sig, siginfo_t *si, void *unused)
       {
           /* Note: calling printf() from a signal handler is not safe
              (and should not be done in production programs), since
              printf() is not async-signal-safe; see signal-safety(7).
              Nevertheless, we use printf() here as a simple way of
              showing that the handler was called. */

           printf("Got SIGSEGV at address: 0x%lx\n",
                   (long) si->si_addr);
           exit(EXIT_FAILURE);
       }
/*Main function*/
int main(int argc, char *argv[])
       {
           char *p,*buffer,*q;
	   char c;
           int pagesize;
	   int i=0,size;
           struct sigaction sa;

           sa.sa_flags = SA_SIGINFO;
           sigemptyset(&sa.sa_mask);
           sa.sa_sigaction = handler;
           if (sigaction(SIGSEGV, &sa, NULL) == -1)
               handle_error("sigaction");

           pagesize = sysconf(_SC_PAGE_SIZE);  /* Initializing Pagesize, here pagesize=4096 Bytes*/
           if (pagesize == -1)
               handle_error("sysconf");

    /* Allocate a buffer; it will have the default
       protection of PROT_READ|PROT_WRITE. */
    size=pagesize*10;
    p = memalign(pagesize,size);          /*Allocating buffer'p' of size = ten pages*/
    if (p == NULL)
    handle_error("memalign");

    memset(p,0x00,size);                     /*Copying 'B' to whole buffer*/
    memset(p,0x41,size);
    
    /* Task 1:Adding my name to the last 5 addresses in pages 9/10 */
    int temp = pagesize * 8;
    i = 0;
    buffer=p;
    *(buffer+temp) = 'F';
    *(buffer+temp+1) = 'n';
    *(buffer+temp+2) = 'a';
    *(buffer+temp+3) = 'm';
    *(buffer+temp+4) = 'e';
    
    for(i=temp;i<(temp+5);i++)
    {
	printf("%c",*(p+i));   /* Printing last 5 bytes from last page(s) */
    } 
    
    printf("\n\n"); // New line to seperate my name from address data
    
    /* Task 2: Adding last name to 5 addresses in pages 7/8 */
    if (mprotect(p + (pagesize*6), pagesize*2, PROT_READ|PROT_WRITE)==-1) /* using mprotect on 7th/8th page to apply read/write access */
    { handle_error("mprotect"); }

    
    temp = pagesize*6;
    i = 0;

    *(buffer+temp+(i)) = 'L';
    *(buffer+temp+(i+1)) = 'n';
    *(buffer+temp+(i+2)) = 'a';
    *(buffer+temp+(i+3)) = 'm';
    *(buffer+temp+(i+4)) = 'e';
    
    for(i=temp;i<(temp+5);i++)
    {
	printf("%c",*(p+i));   /* Printing last 5 bytes from last page(s) */
    } 
    
    printf("\n\n"); // New line to seperate my name from address data
    
    /* Task 3: Set 5th/6th page to write only */
    if (mprotect(p + (pagesize*4), pagesize*2, PROT_WRITE)==-1) /* using mprotect on 7th/8th page to apply read/write access */
    { handle_error("mprotect"); }
    
    temp = pagesize*4;
    i = 0;

    *(buffer+temp+(i)) = 'U';
    *(buffer+temp+(i+1)) = 's';
    *(buffer+temp+(i+2)) = 'e';
    *(buffer+temp+(i+3)) = 'r';
    *(buffer+temp+(i+4)) = 'N';
    *(buffer+temp+(i+5)) = 'a';
    *(buffer+temp+(i+6)) = 'm';
    *(buffer+temp+(i+7)) = 'e';
    *(buffer+temp+(i+8)) = '0';
    
    for(i=temp;i<(temp+9);i++)
    {
	printf("%c",*(p+i));   /* Printing last 5 bytes from last page(s) */
    }
    
    printf("\n\n"); // New line to seperate my name from address data
    
    /* Task 4: Create new buffer, try to copy 7th/8th page to the new buffer */ 
    size=pagesize*2;
    q = memalign(pagesize,size);          /*Allocating buffer'q' of size = two pages*/
    if (q == NULL)
    handle_error("memalign");
    
    // Copy the address values from pages 7/8 to the new buffer zone "q"
    memcpy(q, p + (pagesize*6), pagesize*2);
    
    for(i=0; i<(pagesize*2); i++)
    {
    	if(*(q+i) != 'A')
	printf("%c",*(q+i));   /* Printing all values from new buffer */
    }
    
    printf("\n\n"); // New line to seperate my name from address data
    
    /* Task 5: Copy the 6th and 9th pages to this new buffer */
    if (mprotect(p + (pagesize*5), pagesize, PROT_WRITE)==-1) /* using mprotect on 7th/8th page to apply read/write access */
    { handle_error("mprotect"); }
    
    // Copy the address values from page 6 to the new buffer zone "q"
    memcpy(q, p + (pagesize*5), pagesize);
    
    // Copy the address values from page 9 to the new buffer zone "q"
    memcpy(q + pagesize, p + (pagesize*8), pagesize);
    
    for(i=0; i<(pagesize*2); i++)
    {
    	if(*(q+i) != 'A')
		printf("%c",*(q+i));   /* Printing all values from new buffer */
    }
    
    printf("\n\n"); // New line to seperate my name from address data
    
    /*Printing all pages first  bytes from first page. The usage of %d format specifier causes compilation warnings. Can you figure out why?*/
    for(i=0;i<10;i++)
    {
	printf("Address of Page %d: %lx\n",i+1,p+(i*4096));		
    }

// Can start writing code here and can define variables for functions above


   
           exit(EXIT_SUCCESS);
       }


