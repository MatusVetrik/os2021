// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

//manager of physical memory for allocating
struct {
  struct spinlock lock;
  struct run *freelist;
  uint64 *cntref;	//array where every item respond for one frame
} kmem;


//decrement amount of referenices
uint64 dec_ref(void* pa){
  acquire(&kmem.lock);
  uint64 pa_index = PA2IND(pa);		//parsing physical address to index
  if(kmem.cntref[pa_index] == 0){	//dec_ref cant be use for address with 0 referencies
    panic("dec_ref: cntref zero");
  }
  kmem.cntref[pa_index]--;		//decrementing amount of referncies
  int ret = kmem.cntref[pa_index];
  release(&kmem.lock);
  return ret;		//returning amount of referenices

} 

//increment amount of referenices
void inc_ref(void* pa){
  acquire(&kmem.lock);
  uint64 pa_index = PA2IND(pa);
  if(pa_index>=MAXVA){
    panic("dec_ref: address out of limit");
  } 
  kmem.cntref[pa_index]++;	//increaseing amount of referncies
  release(&kmem.lock);
}

//increment ref count whithout locking
static void inc_ref_internal(void *pa){
  if(PA2IND(pa)>=MAXVA){
    panic("inc_ref_iternal: address out of limit");
  }
  kmem.cntref[PA2IND(pa)]++;
}

//initialisation of allocator
void
kinit()
{
  int frames = 0;			//count of avaiable frames
  uint64 addr = PGROUNDUP((uint64)end);
  
  kmem.cntref = (uint64*)addr;
  while(addr < PHYSTOP){	//walking through free memory and for every frame in cntref is count of referencies set to one
    kmem.cntref[PA2IND(addr)] = 1;
    addr += PGSIZE;
    frames++;
  }
  
  initlock(&kmem.lock, "kmem");
  freerange(kmem.cntref+frames, (void*)PHYSTOP);	//free range of physical memory
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;
  
  //if ref count is equal 0 , continue without iteration
  if(dec_ref(pa) != 0)
    return;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);
    
  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
    
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r){
    kmem.freelist = r->next;
    inc_ref_internal(r);
    memset((char*)r, 5, PGSIZE); // fill with junk
  }
  release(&kmem.lock);
  
  return (void*)r;
}

