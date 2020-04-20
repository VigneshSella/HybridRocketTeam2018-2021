function [P, v] = PVCalc(d,Pint,Vint)
%PVCalc This function calculates the Pressure and Velocity through a pipe.
%   Vignesh Sella
%   11/22/2018
%Inputs:
%   d = diameter
%   Pint = Initial Pressure
%   Vint = Initial Velocity
%Outputs:
%   P = Downstream Pressure
%   V = Downstream Velocity
global mdot
global rho
    a = (pi/4)*d^2;
    v = mdot/(a*rho);
    P = Pint + 1/2*rho*Vint^2 - 1/2*rho*v^2;
end
