 /*
 * *
 * * Filename: SharedMemory.hpp
 * *
 * * Description:
 * *   System V Shared Memory wrapper class. Shared memory segments are pages or portions/blocks
 * *     of memory where information can be stored and shared between processes. This class
 * *     provides methods to create, attach, detach, remove and query shared memory segments.
 * *
 * *  Author:
 * *    JEP  J. Enrique Peraza
 * *
 */
#pragma once
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <string.h> // memset

namespace ipc
{
  // Result structure for shm operations
  struct ShmResult
  {
    int code;                           // >=0 success, <0 error
    int errn;                           // errn=errno snapshot
  };                                    // ShmResult
  // Access flags
  enum
  {
    SHM_PERM_600=0600,                  // user read/write
    SHM_PERM_644=0644                   // user read/write, group/others read
  } SHM_PERMS;                          // Permission flags
  // Mode flags
  enum
  {
    SHM_CREATE=IPC_CREAT,               // create if key does not exist
    SHM_EXCL=IPC_EXCL                   // fail if key exists
  } SHM_MODES;                          // Creation flags
  // Key maker (blacksmith ftok wrapper)
  inline key_t MakeKey(
    const char* path,                   // The path to the file
    int pjid)                           // The project identifier
  {                                     // ~~~~~~~~~ MakeKey ~~~~~~~~~  
    return ftok(path,pjid);             // Return the key
  }                                     // ~~~~~~~~~ MakeKey ~~~~~~~~~
  // Shared Memory Object
  class SharedMemory
  {
    public:
      SharedMemory (void)=default;
      ~SharedMemory (void)
      {
        Detach();
        if(owner)
          Remove();
      }
      // Create or attach to a shared memory segment
      ShmResult CreateOrAttach (
        key_t key,                      // The key for the segment
        size_t size,                    // The size of the segment
        int mode=SHM_PERM_600,          // The access mode
        int flags=SHM_CREATE)           // The creation flags
      {                                 // ~~~~~~~~~ CreateOrAttach ~~~~~~~~~     
        if (shmid>=0)                   // ID already initialized?
          return {shmid,0};             // yes, return it with no error code.
        int id=shmget(key,size,mode|flags);// Get the segment ID
        if (id<0)                       // Error?
          return {-1,errno};            // yes, return error code
        shmid=id;                       // Otherwise store the ID.
        memsize=size;                   // Store the size
        // Is it a new segment?
        owner=(flags&IPC_EXCL)?true:false;// Set owner flag
        return {shmid,0};               // Return the ID with no error code
      }                                 // ~~~~~~~~~ CreateOrAttach ~~~~~~~~~
      ShmResult Attach (
        int shmatt=0)                   // The attach flags
      {                                 // ~~~~~~~~~ Attach ~~~~~~~~~~~~~~
        if (shmid<0)                    // ID not initialized?
          return {-1,EINVAL};           // Yes, we cant attach. Error out.
        void* p=shmat(shmid,nullptr,shmatt);// Attach to the segment
        if (p==(void*)-1)               // Error?
          return {-1,errno};            // Yes, return error code
        memptr=p;                       // Store the pointer
        return {shmid,0};               // Return success
      }                                 // ~~~~~~~~~ Attach ~~~~~~~~~~~~~~
      ShmResult Detach (void)
      {                                 // ~~~~~~~~~ Detach ~~~~~~~~~~~~~~
        if (memptr!=nullptr)            // Are we attached to anything?
        {                               // Yes, so we will...
          if (shmdt(memptr)!=0)         // Try to detach from the segment
            return {-1,errno};          // Error? Yes, return error code
          memptr=nullptr;               // Clear the pointer
        }                               // No, we have detached or were never attached
        return {0,0};                   // Return success
      }                                 // ~~~~~~~~~ Detach ~~~~~~~~~~~~~~ //
      // Remove the shared memory segment (only if owner)
      ShmResult Remove (void)
      {                               // ~~~~~~~~~ Remove ~~~~~~~~~~~~~~
        if (shmid<0)                  // ID not initialized?
          return {-1,EINVAL};         // Yes, we cant remove. Error out.
        if (shmctl(shmid,IPC_RMID,nullptr)!=0)// Try to remove the segment
          return {-1,errno};          // Error? Yes, return error code
        shmid=-1;                     // Clear the ID
        memptr=nullptr;               // Clear the pointer
        memsize=0;                    // Clear the size
        owner=false;                  // Clear the owner flag
        return {0,0};                 // Return success
      }                               // ~~~~~~~~~ Remove ~~~~~~~~~~~~~~
      // Mark for removal (segment removed when last detach happens)
      ShmResult MarkForRemoval (void)
      {                               // ~~~~~~~~~ MarkForRemoval ~~~~~~~~~~
        if (shmid<0)                  // ID not initialized?
          return {-1,EINVAL};         // Yes, we cant mark what we do not know.
        if (shmctl(shmid,IPC_RMID,nullptr)!=0)// Try to mark the segment for death.
          return {-1,errno};          // Error? Yes, return error code
        return {0,0};                 // Return success
      }                               // ~~~~~~~~~ MarkForRemoval ~~~~~~~~~~
      // Query size/perm/owner/etc.
      ShmResult Stat (
        struct shmid_ds* buf) const   // The buffer to fill with the info
      {                               // ~~~~~~~~~ Stat ~~~~~~~~~~~~~~
        if (shmid<0)                  // ID not initialized?
          return {-1,EINVAL};         // Yes, we cant stat what we do not know.
        if (shmctl(shmid,IPC_STAT,buf)!=0)// Try to get the segment info
          return {-1,errno};          // Error? Yes, return error code
        return {0,0};                 // Return success
      }                               // ~~~~~~~~~ Stat ~~~~~~~~~~~~~~ //
      // Accessors
      void* GetPointer (void) const { return memptr; }
      size_t GetShmSize (void) const { return memsize; }
      int GetShmId (void) const { return shmid; }
      bool IsOwner (void) const { return owner; }
    private:
      int shmid{-1};                  // Shared memory ID
      void* memptr{nullptr};          // Pointer to the memory region
      size_t memsize{0};              // Size of the memory region
      bool owner{false};              // True if this instance created the segment
  };
}