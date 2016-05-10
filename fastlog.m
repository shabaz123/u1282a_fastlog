%% fastlog.m
% revision 1.0 May 2016 shabaz
%

%% Adjust these to suit
clear all;
port='com4'; %serial port
iter=1200;

% labels for the chart
mytitle='Pi 3 Temperature Measurement';
myyaxis = sprintf('Temperature (%cC)', char(176));

%% equation related constants and variables
coefa = 7.987407898E-4
coefb = 2.125901507E-4
coefc = 7.077938365E-8

%% some constants
onesec=1.157407407E-5

%% initialization
% close any ports previously left in an open state
ports=instrfind;
if isempty(ports)==0
  fclose(ports);
  delete(instrfind);
end

% open up serial port
s = serial(port)
set(s, 'Terminator', 'LF');
fopen(s);

% read some basic information from the U1282A
fprintf(s, '%s\r\n', '*IDN?', 'async');
pause(0.2);
sline=fgetl(s);
disp(sline);
fprintf(s, '%s\r\n', 'CONF?', 'async');
pause(0.2);
sline=fgetl(s);
disp(sline);
fprintf(s, '%s\r\n', 'STAT?', 'async');
pause(0.2);
sline=fgetl(s);
disp(sline);
fprintf(s, '%s\r\n', 'SYST:BATT?', 'async');
pause(0.2);
sline=fgetl(s);
disp(sline);

time=now;
meas=0;


% create a figure window
figureHandle = figure('NumberTitle','off',...
    'Name',mytitle,...
    'Color',[0.92 0.92 0.92],'Visible','off');

axesHandle = axes('Parent',figureHandle,...
    'YGrid','on',...
    'YColor','black',...
    'XGrid','on',...
    'XColor','black',...
    'Color','white');

hold on;


% set x-axis limits to current time and current time+10sec
%xlim(axesHandle,[time time+(onesec*10)]);
% set x-axis limits to current time and current time+(iter/10)
xlim(axesHandle,[time time+(onesec*(iter/10))]);

title(mytitle,'FontSize',15,'Color','black');
xlabel('Time','FontWeight','normal','FontAngle','normal','FontSize',12,'Color','black');
ylabel(myyaxis,'FontWeight','normal','FontAngle','normal','FontSize',12,'Color','black');
plotHandle = plot(axesHandle,time,meas,'Marker','none','LineWidth',1,'Color','blue');


%% main routine

meas(1)=0;
time(1)=time;
count = 2;
k=1;
while ~isequal(count,iter)
    
    %read value from U1282A
    fprintf(s, '%s\r\n', 'FETC?', 'async');
    pause(0.1);
    sline=fgetl(s);
    C = textscan(sline, '%f');
       
    skip=0;
    if isempty(C{1})
         skip=1;
    end
     
    if skip==0
        meas(count) = C{1};    

        %equation; delete this section if raw readings are desired
        lnr=log(meas(count));
        ek=1.0/(coefa+(coefb*lnr)+(coefc*lnr^3));
        meas(count)=ek-273.15;
        % end of equation
        
        meas(1)=meas(2);
        time(count) = time(count-1)+(onesec/10);
        set(plotHandle,'YData',meas,'XData',time);
        set(figureHandle,'Visible','on');
        datetick('x','HH:MM:SS');
        count = count +1;
    end
end


%% close the serial port
fclose(s);
delete(s);
clear s;
