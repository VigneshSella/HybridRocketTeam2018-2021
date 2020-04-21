%% Nozzle_heat_transfer.m

%{
Created: Nicolas Rasmont
December 5, 2018

Updated: Andrew Larkey
April 20, 2020

Illinois Space Society 
Hybrid Rocket Project
Nozzle Heat Transfer Calculations and Analysis

%}

clc;clear;
close all;

X = linspace(0,0.120,100);
%% Engine Properties
T_0 = 3700;
P_0 = 4E6;
rho_0 = 3.61;
gamma = 1.14;
r = 0.91;% 

%% Gas Properties
mu_N2 = 65E-6;
k_N2 = 0.11;
mu_CO = 26E-6;
k_CO = 0.04;
mu_H2O = 48E-6;
k_H2O = 0.1138;
mu_H2 = 20.35E-6;
k_H2 = 0.526;


MU = [mu_N2 mu_CO mu_H2O mu_H2];
K = [k_N2 k_CO k_H2O k_H2];

r_N2 = 0.525;
r_CO = 0.199;
r_H2O = 0.192;
r_H2 = 0.084;

R = [r_N2 r_CO r_H2O r_H2];

M_molar = [28 28 18 2];

R_mass = R.*M_molar./(R*M_molar.');

%% Averaged properties
mu = MU*R_mass.'
k = K*R_mass.'

A = nozzle(X);

M = Mach(A,gamma);

T = [X; T_0./(1+(gamma-1)/2*M(2,:).^2)];

T_adia = [X; T_0*(r.*ones(1,length(X)) + (1-r)./(1+(gamma-1)/2*M(2,:).^2))];

P = [X; P_0./(1 + (gamma-1)/2*M(2,:).^2).^(gamma/(gamma-1))];
    
rho = [X; rho_0./(1 + (gamma-1)/2*M(2,:).^2).^(gamma/(gamma-1))];

V =  [X; M(2,:).*(gamma*P(2,:)./rho(2,:)).^0.5];

C_p = SpecificHeat(A);

h = [X ; 0.026*k*(rho(2,:).*V(2,:)./mu).^(0.8).*(1./((4/pi*A(2,:)).^0.5)).^(0.2).*(C_p(2,:)*mu/k).^(0.4)]; 

figure
plot(rho(1,:),rho(2,:))
xlim([0,0.12])
%ylim([0,3])
ylabel('Density (kg/m^-3)')
xlabel('Axial position in the nozzle (m)')

figure
plot(T(1,:),T(2,:),T_adia(1,:),T_adia(2,:))
xlim([0,0.12])
ylim([2000,3800])
legend('Static flow temperature (K)','Adiabatic wall temperature (K)','Location','southwest')
ylabel('Temperature (K)')
xlabel('Axial position in the nozzle (m)')

figure
plot(h(1,:),h(2,:))
xlim([0,0.12])
ylabel('Convective heat transfer coefficient (W/m^2/K)')
xlabel('axial position in the nozzle (m)')

function Y = nozzle(x)
for i=1:length(x)
if x(i) < 0.00704
    
    y(i) = 0.02964-x(i); 
    
else if (x(i)>= 0.00704)&&(x(i)<= 0.03091)
        
       y(i) = -sqrt(0.02471^2-(x(i)-0.02451)^2) + 0.04000;
       
    else if x(i) > 0.03091
            y(i) = 0.26795*(x(i)-0.03091)+0.01613;
        end
    end
end            
end
A = pi*y.^2;
Y = [ x ; A ];
end

function M = Mach(A,k)
a_star = min(A(2,:));
%index_a_star = find(A(2,:)==a_star);
m = zeros(1,length(A(1,:)));
for i=1:length(A(1,:))
    a = A(2,i);
    fcn = @(z) (1/z)*((2+(k-1)*z^2)/(k+1))^((k + 1)/(2*(k - 1))) - a/a_star;
    if (i>2)&&((A(2,i)-A(2,i-1))>=0)
        m(i) = fzero(fcn,15);
    else
        m(i) = fzero(fcn,1);
    end
end
 M = [A(1,:); m];
end

function Cp = SpecificHeat(A)

    index_a_star = find(A(2,:)==min(A(2,:)));
    Cp_0 = 4841;
    Cp_t = 4494;
    Cp_e = 2103;
    
    for i=1:index_a_star
        Cp(1,i)= A(1,i);
        Cp(2,i) = Cp_0-A(1,i)/A(1,index_a_star)*(Cp_0-Cp_t);
    end
    for i=(index_a_star+1):length(A(2,:))
        Cp(1,i)=A(1,i);
        Cp(2,i)= Cp_t - (A(1,i)-A(1,index_a_star))/(A(1,end) - A(1,index_a_star))*(Cp_t - Cp_e);
    end 
end

% function M = mach2(A,k)
% 
% M = linspace(0,4,length(A(1:);
% Alpha = (1./M).*((2+(k-1)*M.^2)/(k+1)).^((k + 1)/(2*(k - 1)));
% A_reorg = Alpha*min(A(2,:));
% Mach = [M; A_reorg ]
% 
% index_sonic = find(Mach(1,:)==1);
% 
% Mach_sub = Mach(:,1:index_sonic);
% 
% Mach_super = Mach(:,(index_sonic+1):end);
% 
% for i=1:
% 
% end
