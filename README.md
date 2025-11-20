# Multi-thread-synchronization-using-semaphores-Asmart2022

## Solution 1
![Problem 1](Problem1.png)

No-Starve Reader-Writer: This solution prevents the writers from starving by using a turnstile semaphore that locks a writer and prevents new readers from entering until the current reader finishes. This is done by using a lightswitch which batches the reader, roomEmpty that are used for writers and the turnstile.

## Solution 2
![Problem 2](Problem2.png)

Writer Priortiy Reader-Writer: This solution gives the writers priority, making sure that if a writer is active, no new readers can enter. This solution uses 2 lightswitches for the reader and writer, and two semaphores that block readers and moderate writers, giving a strict priority

## Solution 3
![Problem 3](Problem3.png)

Dining Philosophers #1: This solution prevents deadlocks, using a multiplex semaphore to allow 4 philosophers to be at the table, with the fork being availiable for each to use, allowing them to not need circular logic to starve them.

## Solution 4
![Problem 4](Problem4.png)

Dining Philosophers #2: This solution lacks the footman of the previous solution, but is solved by using the number to determine the order the forks are picked up, with the order able to break the cycle and prevent a deadlock
