function [A2, V_inj, N, A2_tot] = OrfG(mdot, x)
% OrfG This function calculates the required orifice geometry for a given mdot
%   Vignesh Sella
%   12/3/2018
%Inputs:
%     mdot = mass flow rate
%Outputs
%   A2 = Area of an individual orifice
%   V_inj = Velocity directly after injection
%   N = # of injectors
%   A2_tot = Total area of all orifices
global d
global rho
global k
global deltap
A2 = pi*(d^2/4); %m^2 // Area of an individual orifice 
V_inj = sqrt((2*deltap)/(k*rho)); %m/s // Velocity of fluid through injector **ALSO EQUAL TO v3/sqrt(k)**
n_temp = mdot ./(rho*V_inj*A2); %# // Number of orifices in injector plate
if x == 0
    y = 0;
else 
    y = 1;
end  
N = ceil(n_temp) + y;
A2_tot = N*A2;
end

