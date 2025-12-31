# Order Management System (C)

University project developed individually in C.

The project implements a discrete-time simulation of an order management system
for an industrial bakery, including recipe handling, inventory management,
order processing, and periodic delivery scheduling.

---

## Project Overview

The system simulates the internal workflow of a bakery operating in discrete time.
At each time step, the program processes commands related to recipes, inventory
restocking, and customer orders, while periodically loading ready orders onto a courier
with limited capacity.

The implementation focuses on:
- efficient data structures
- correct handling of constraints defined by the specification
- time and memory efficiency

---

## Main Features

- **Recipe Management**
  - Add and remove recipes composed of multiple ingredients
  - Each recipe stores ingredient quantities and total weight

- **Inventory Management**
  - Ingredients stored in batches with expiration dates
  - FEFO policy (First-Expired, First-Out) for ingredient consumption
  - Automatic removal of expired inventory

- **Order Processing**
  - Immediate order preparation when ingredients are available
  - FIFO queue for pending orders
  - Automatic order unlocking after restocking

- **Courier Scheduling**
  - Periodic courier with fixed capacity
  - Orders loaded by decreasing weight and increasing arrival time
  - Output formatted according to specification

- **Discrete-Time Simulation**
  - Time advances with each processed command
  - Periodic events handled deterministically

---

## Technologies

- **Language**: C (GNU C11)
- **Standard Libraries**: stdio, stdlib, string
- **Data Structures**:
  - Hash tables with chaining
  - Linked lists
  - FIFO queues
  - Dynamically allocated arrays

---

## Data Structures Overview

- **Recipes**
  - Stored in a hash table of size 65537
  - Each recipe contains a linked list of ingredients
  - Precomputed hash indices used for faster lookup

- **Inventory**
  - Hash table indexed by ingredient name
  - Each ingredient maintains a sorted list of batches by expiration date

- **Orders**
  - FIFO queue storing both pending and ready orders
  - Orders carry weight, quantity, timestamp, and status

- **Courier Load**
  - Temporary linked list sorted using a custom quicksort
  - Sorting criteria: weight (descending), time (ascending)

---

## Input Format

The program reads input from **standard input**.

### Initial Line

