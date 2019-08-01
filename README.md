# V2V-Simulator-App

This project was created as part of a research project by the Institute for Software Research at Carnegie Mellon University. For more detailed documention, read the Veins Apps Docs PDF included with the project. The objective of the project is to study how we can maintain safety when failures or delays in vehicle-to-vehicle communication networks occur.  

### Description ###
This project simulates a network between cars and an RSU (road side unit) at a four-way intersection. The cars send messages to the RSU with their current road ID, which direction they want to turn, and other necessary information. The RSU will figure out which vehicles are allowed to go based on normal intersection rules and then send a message containing a list of vehicles allowed to go out to all the vehicles. If the vehicle is allowed to go, then it will proceed through the intersection. Otherwise, it will slow down and stop at the intersection and wait until it is allowed to go. 

The project uses an open-source traffic simulator called SUMO, an event-based network simulator called OMNeT++, and a vehicle  network simulator called Veins which is built on top of these two frameworks and connects them. Veins runs the two simulators in parallel, connected with a TCP socket. The movement of the cars in the SUMO simulation will correspond with the movement of nodes in the OMNeT++ simulation. This is all handled through a part of SUMO known as TraCI (Traffic Control Interface). TraCI gives access to a running SUMO simulation and allows you to change its behavior dynamically. The TraCI commands have been implemented in Veins. Veins is designed to be the basis of writing application-specific simulation code.

