# -*- coding: utf-8 -*-

import random
import numpy as np

TAMANHO = 50

TEXTO = ""
       
matrix = np.zeros((TAMANHO, TAMANHO)).astype(int)

for i in range(TAMANHO):
    
    for j in range(i, TAMANHO):
        
        if (i == j):
            pass
        else:
            
            p = random.randint(0, 3)
            
            if(p == 0):
                
                pass
            else:
                
                n = random.randint(0, 1000)
                
                matrix[i][j] = n
                matrix[j][i] = n

path = "grafos/" + str(TAMANHO)
arquivo = open(path, 'w')

for i in range(TAMANHO):
      
    for j in range(TAMANHO):
        
        num = matrix[i][j]
        arquivo.write(str(num) + '\n')

arquivo.close()                       
            
print "fim"
