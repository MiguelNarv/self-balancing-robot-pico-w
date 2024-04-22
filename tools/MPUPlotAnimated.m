clear;

%% Establish TCP/IP connection with pico server
t= tcpclient('192.168.1.234',4242,"ConnectTimeout",7);
configureTerminator(t,"CR/LF")

%% Plot received buffer
RECORDING_TIME = 30;
TRANSMITION_PERIOD = 0.02;
counter = 0.0;
index = 1;

% Figure properties.
f = figure;
subplot(3,1,1);
yAngXAxis = animatedline('Color','b');
title('Robot measurements')
legend('Y angle')
xlabel('Time [s]') 
ylabel('Angle [deg]') 

subplot(3,1,2);
rightSpeedXAxis = animatedline('Color','r');
legend('Right wheel speed')
xlabel('Time [s]') 
ylabel('Speed [rad/s]') 

subplot(3,1,3);
leftSpeedXAxis = animatedline('Color','g');
legend('left wheel speed')
xlabel('Time [s]') 
ylabel('Speed [rad/s]') 

% Initialize recording buffers.
recordedTime = zeros(1, RECORDING_TIME/TRANSMITION_PERIOD);
recordedYAngle = zeros(1, RECORDING_TIME/TRANSMITION_PERIOD);

% Plot RECORDING_TIME of received data.
while counter < RECORDING_TIME
        
    % If a message is received from server do the animated plot.
    if t.NumBytesAvailable > 0
        
        % Format received buffer.
        rxBuffer = strsplit(readline(t), ",");
        
        yAng = str2double(rxBuffer{1});
        rightSpeed = str2double(rxBuffer{2});
        leftSpeed = str2double(rxBuffer{3});
        
        % Add points.
        addpoints(yAngXAxis,counter,yAng);
        addpoints(rightSpeedXAxis,counter,rightSpeed);
        addpoints(leftSpeedXAxis,counter,leftSpeed);

        recordedTime(index) = counter;
        recordedYAngle(index) = yAng;
        
        % Update screen
        drawnow limitrate
    
    % Increment for desired analysis time, based on pico transmit 
    % period.
    counter = counter + TRANSMITION_PERIOD;
    index = index + 1;
    end
        
end
%% Save recorded data
% Save matrix in current folder as .xlsx.
writematrix([recordedTime; recordedYAngle], "Robot_recordings.xlsx");

%% Close TCP/IP
% Connection with server not closed.
%close(f1)
clear t
echotcpip("off")

