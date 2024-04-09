clear;

%% Establish TCP/IP connection with pico server
t= tcpclient('192.168.1.234',4242,"ConnectTimeout",7);
configureTerminator(t,"CR/LF")

%% Plot received buffer
RECORDING_TIME = 10;
TRANSMITION_PERIOD = 0.02;
counter = 0.0;
index = 1;

% Figure properties.
f = figure;
y_ang_xaxis = animatedline('Color','b');
title('Robot angles')
legend('X angle')
xlabel('Time [s]') 
ylabel('Angle [deg]') 

% Initialize recording buffers.
recordedTime = zeros(1, RECORDING_TIME/TRANSMITION_PERIOD);
recordedYAngle = zeros(1, RECORDING_TIME/TRANSMITION_PERIOD);

% Plot RECORDING_TIME of received data.
while counter < RECORDING_TIME
        
    % If a message is received from server do the animated plot.
    if t.NumBytesAvailable > 0

        % Format received buffer.
        y_ang = str2double(readline(t));
        
        % Add points.
        addpoints(y_ang_xaxis,counter,y_ang);
        
        recordedTime(index) = counter;
        recordedYAngle(index) = y_ang;
        
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

