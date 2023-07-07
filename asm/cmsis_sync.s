.syntax unified

@ https://developer.arm.com/documentation/dht0008/a/arm-synchronization-primitives/practical-uses/implementing-a-mutex?lang=en
@ https://developer.arm.com/documentation/dht0008/a/arm-synchronization-primitives/practical-uses/implementing-a-semaphore?lang=en

.equ locked, 1
.equ unlocked, 0

.macro WAIT_FOR_UPDATE
wfe
.endm

.macro SIGNAL_UPDATE
dsb
sev
.endm


@ lock_mutex
@ Declare for use from C as extern void lock_mutex(void * mutex);
.global lock_mutex

lock_mutex:
    ldr     r1, =locked
1:  ldrex   r2, [r0]
    cmp     r2, r1          @ Test if mutex is locked or unlocked
    beq     2f              @ If locked - wait for it to be released, jump to label 2
    strexne r2, r1, [r0]    @ Not locked, attempt to lock it
    cmpne   r2, #1          @ Check if Store-Exclusive failed
    beq     1b              @ Failed - retry from label 1
    # Lock acquired
    dmb                     @ Required before accessing protected resource
    bx      lr

2:  @ Take appropriate action while waiting for mutex to become unlocked
    WAIT_FOR_UPDATE
    b       1b              @ Retry from label 1


@ unlock_mutex
@ Declare for use from C as extern void unlock_mutex(void * mutex);

.global unlock_mutex
unlock_mutex:
    ldr     r1, =unlocked
    dmb                     @ Required before releasing protected resource
    str     r1, [r0]        @ Unlock mutex
    SIGNAL_UPDATE
    bx      lr

.global sem_dec

sem_dec:
1:  LDREX   r1, [r0]
    CMP     r1, #0         @ Test if semaphore holds the value 0
    BEQ     2f             @ If it does, block before retrying
    SUB     r1, #1         @ If not, decrement temporary copy
    STREX   r2, r1, [r0]   @ Attempt Store-Exclusive
    CMP     r2, #0         @ Check if Store-Exclusive succeeded
    BNE     1b             @ If Store-Exclusive failed, retry from start
    DMB                    @ Required before accessing protected resource
    BX      lr

2:  @ Take appropriate action while waiting for semaphore to be incremented
    WAIT_FOR_UPDATE        @ Wait for signal to retry
    B       1b

.global sem_inc
sem_inc:
1:  LDREX   r1, [r0]
    ADD     r1, #1         @ Increment temporary copy
    STREX   r2, r1, [r0]   @ Attempt Store-Exclusive
    CMP     r2, #0         @ Check if Store-Exclusive succeeded
    BNE     1b             @ Store failed - retry immediately
    CMP     r0, #1         @ Store successful - test if incremented from zero
    DMB                    @ Required before releasing protected resource
    BGE     2f             @ If initial value was 0, signal update
    BX      lr

2:  @ Signal waiting processors or processes
    SIGNAL_UPDATE
    BX      lr

