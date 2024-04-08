clear;

%% Establish TCP/IP connection with pico server
t= tcpclient('192.168.1.234',4242,"ConnectTimeout",7);
configureTerminator(t,"CR/LF")

%% Plot received buffer
counter = 0.0;
f = figure;
x_raw = animatedline('Color','r');
x_filtered = animatedline('Color','b');

%%% Plot 30s of received data
while counter < 30
        
        %%% If a message of the transmit buffer size is received from
        %%% server do the animated plot.
        if t.NumBytesAvailable > 11
            %%% Spit and format received buffer.
            rxArray = strsplit(readline(t), ',');
            x_ang = str2double(rxArray{1});
            x_ang_filtered = str2double(rxArray{2});
            
            % Add points.
            addpoints(x_raw,counter,x_ang);
            addpoints(x_filtered,counter,x_ang_filtered);
            
            legend('X angle','Y angle')

            % Update screen
            drawnow limitrate
        else
            %%% Period of data received, based on pico transmit period.
            pause(0.02)
        end
         %%% Innrement for desired analysis time, based on pico transmit 
         %%% period.
        counter = counter + 0.02;
end


%% Close TCP/IP
% Connection with server not closed.
%close(f1)
clear t
echotcpip("off")

