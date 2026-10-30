# Plan
- package launch file to launch turtle sim

Nodes
- turtlesim (aready built in)
- turtle_controller
    - 
- turtle_spawner
    - get the dimension of the turtlesim
    - create a timer event with frequency to spawn the turtle
    - create a function to spawn turtle within the turtlesim dimension

    - array of alive turtle
    - have a callback when turtlecontorll send signal, update the array

- turtle_controller
    - find distance
    - find angle
    - move to turtle
    - kill turtle when reached (send this to turtle_spawner)

