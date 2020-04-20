clear all
clc
format long

Cd = .75;
A = 3.053628059289279e-05;
A3 = 3.0631e-05;
A2 = pi/4 * .03^2;
% Specific Enthalpy = Molar Enthalpy/Molecular Weight (i.e 44.013 kg/Kmol)
h1 = 1614;
%h2 = 1579;
% h1 = 6.970e4;
%h2 = 6.897e4;
h2 = h1 - 1/2*(67.395)^2
rho_1 = 769.9;
rho_2 = 407.8;

gamma = 1.126;
Cp=124.2;
P2 = 4e6;
P1 = 7.5e6;
Pv = 4.86e6;
C = .15;
Ccv = 1;
K = sqrt((7.5e6-4e6)/(Pv-4e6));
k = (768.9/105)^(1/3);
k2 = (851/115.8)^(1/3);
x2 = .1712;


mHEM = Cd*A*rho_2*sqrt(2*(h1-h2))
mSPI = Cd*A*sqrt(2*rho_1*3.49*10^6)
mDYER = ((K/(1+K))*mSPI + ((1/(1+K))*mHEM));
mPG = Cd*A*rho_1*sqrt(2*Cp*25*[(P2/P1)^(2/gamma) - (P2/P1)^((gamma+1)/gamma)]);
mPGcrit = Cd*A*sqrt(gamma*rho_1*P1*(2/(gamma+1))^((gamma+1)/(gamma-1)));
mBURN = Cd*A*sqrt(2*rho_1*(P1 - (1 - C)*Pv))
mZAL = Ccv*Cd*A*sqrt(2*rho_1*(P1-Pv))
mPROP = Ccv*Cd*A*sqrt(2*rho_1*(P1 - (1-C)*Pv))
mMOOD = Cd*A*(k/(x2 + k*(1-x2)*(115.8/851)))*115.8*sqrt((2*(h1 - h2))/(x2*(k2^2-1)+1))
mBAB = Cd*A*sqrt(2*rho_2*(P1-P2))