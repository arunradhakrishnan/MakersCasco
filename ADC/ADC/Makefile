#MANUAL CMD:
#g++ -Wall -w -o helmet.out helmet_app.cpp GPS.cpp TinyGPS.cpp gas.cpp pwm.cpp gpio.cpp Accelerometer.cpp I2C.cpp algoritmos.hpp -lfann -lm

# Object files to either reference or create                          
OBJECTS = wrist_band.cpp 

# The executable file that will be created at the end                 
EXEC = wrist.out

# The flags to use for compilation                                    
PREFIX_FLAGS = -w
POSFIX_FLAGS = -lm

# The code compiler to use for compilation                            
CC = g++

# Perform action on all object files (May or may not exist)
all: $(OBJECTS)
	$(CC) $(PREFIX_FLAGS) -o $(EXEC) $(OBJECTS) $(POSFIX_FLAGS)
