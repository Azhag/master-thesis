%function [ output_args ] = rate_reaction_success( input_args )
%RATE_REACTION_SUCCESS Summary of this function goes here
%   Detailed explanation goes here

all = load('reactions_successrate4_50ex');

nb_reactions = 6;
nb_encounters = zeros(1,nb_reactions);
nb_successfull_assembly = zeros(1,nb_reactions);


for i=1:nb_reactions,
    reactions{i} = all(all(:,1) == i,2);
    
    nb_encounters(i) = size(reactions{i},1);
    nb_successfull_assembly(i) = sum(reactions{i});
end;

ratios_sucess = nb_successfull_assembly./nb_encounters