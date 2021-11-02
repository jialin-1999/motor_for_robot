echo "Mange motor"
roslaunch motor motor_manager.launch &
sleep 2
echo "Setup the command topic"
roslaunch motor motor_command.launch &
echo "Run motor node"
sleep 1
rosrun motor motor
