\documentclass[article]{rian_article}

\author{Wagner de Lima\\ \email{waglds@gmail.com}\And 
        Ramon Santos\\ \email{ramonsantos.pe@gmail.com}}

\Plainauthor{Wagner de Lima \& Ramon Santos}

\title{Comparação de Abordagens Serial e Paralela em GPU, e Branch-and Bound Aplicado ao Problema das $p$-medianas}
\Plaintitle{Comparação de Abordagens Serial e Paralela em GPU, e Branch-and Bound Aplicado ao Problema das $p$-medianas}
\Shorttitle{Problema $p$-medianas} 

\Abstract{

    Este trabalho trata do problema das $p$-medianas. Nele foram implementadas três abordagem diferentes para encontrar as soluções das instâncias deste problema, todas implementadas usando a linguagem C++. A primeira abordagem foi um método serial que verifica todas as possíveis soluções da instância. O segundo método foi uma implementação em paralelo, através da plataforma CUDA, que também explora todas as possíveis soluções das instâncias do problema. Na terceira abordagem, o problema foi modelado em programação linear, e foi usada uma conhecida implementação do método Branch-and-Bound, o Cplex, para calcular as instâncias. Os resultados mostram que a implementação paralela obteve um speed-up de 35.297x em relação a implementação em serial. No entanto, a abordagem usando o Cplex foi a que teve o melhor desempenho dentre as estratégias avaliadas.
}

\Keywords{$p$-medianas, CUDA, GPU, Métodos Exatos}
\Plainkeywords{$p$-medianas, CUDA, GPU, Métodos Exatos} 

\Versao{3}
\Year{2015}
\Semestre{2}
\Month{Novembro}
\Submitdate{\today}
% \Acceptdate{2012-06-04}
\Disciplina{Programação Matemática}

%%%%%%%%%%%%%%%%%%%%%%%%%% PACOTES %%%%%%%%%%%%%%%%%%%%%%%%%%
\usepackage{amsmath}
\usepackage{bbm}
\usepackage{float}
\usepackage{subfig}
\usepackage{indentfirst}
\usepackage[T1]{fontenc}
\usepackage{url,bm}
\usepackage{textcomp}
\usepackage{graphicx}
\usepackage{amssymb}
\usepackage{anysize}
\usepackage{algpseudocode}
\usepackage{algorithm}
\usepackage{amsthm}
\usepackage{tikz}
\usepackage{enumerate}
\usepackage{table}

%%%%%%%%%%%%%%%%%%%%%%%%% Comandos %%%%%%%%%%%%%%%%%%%%%%%%%%%%
\newcommand{\Cpp}{C\nolinebreak\hspace{-.05em}\raisebox{.4ex}{\tiny\bf +}\nolinebreak\hspace{-.10em}\raisebox{.4ex}{\tiny\bf +}~} 
\newcommand{\NPcom}{$\mathcal{NP}$-completo}
\newtheorem{teorema}{Teorema}[section]
\newtheorem{defi}{Definição}[section]
\floatname{algorithm}{Algoritmo}
\newenvironment{prova}{\begin{proof}[\bf\textsc{Prova.}]}{\end{proof}}

\begin{document}

%%%%%%%%%%%%%%%%%%%%%%%%%% Introdução %%%%%%%%%%%%%%%%%%%%%%%%%%


\section{Introdução}

    O principal objetivo em um possível problema de localização é estabelecer locais onde serão sediadas facilidades viáveis para assistir um conjunto de população da melhor maneira possível. Talvez o problema da $p$-mediana seja o mais clássico no que refere-se à localização, além disso, é também conhecido e utilizado por empresas e órgãos governamentais para a localização de pontos estratégicos para a construção de escolas, hospitais, depósitos e etc, o que minimizaria do custo de acesso a tais facilidades.
    
    Este trabalho irá implementar o problema das $p$-medianas de forma paralela usando a plataforma CUDA. O objetivo é investigar se esse método paralelo terá um desempenho melhor ou pior do que uma implementação do método Branch-and-Bound (Cplex) para o mesmo problema.
    
    Com a introdução de \emph{CUDA}, a \emph{NVIDIA} endereçou que \textit{GPUs} podiam ser usadas para propósitos mais gerais em diversos campos \cite{GPU2015}. Atualmente a maioria dos códigos de simulações computacionais são escritas para placas \textit{GPUs}, utilizando-se de \textit{CUDA}, uma plataforma de computação massivamente paralela baseada em C/C++, incluindo algumas extensões que executam funções em placas gráficas da \textit{NVIDIA}.
   
    Este trabalho está organizado da seguinte maneira: a Seção \ref{sec:secao_prob_p_mediana} discute o problema da $p$-mediana, assim como também descreve sua formulação matemática; a Seção \ref{sec:metodologia} apresenta alguns dos possíveis métodos para a solução do problema das $p$-medianas; a Seção \ref{sec:proposta} descreve a proposta deste trabalho; a Seção \ref{sec:exp_comp} mostra os resultados dos experimentos; a Seção \ref{sec:conclusão} apresenta as considerações finais do trabalho.


%%%%%%%%%%%%%%%%%%%%%%%%%% Problema $p$-mediana %%%%%%%%%%%%%%%%%%%%%%%%%%


\section{Problema $p$-mediana}
    \label{sec:secao_prob_p_mediana}

    Os \emph{Problemas de Localização de Facilidades} (ou \emph{problem of locating p facilities} em inglês) têm como objetivo decidir, da forma mais otimizada possível e de acordo com critérios preestabelecidos, onde serão sediadas certas facilidades. Entende-se como facilidade, qualquer local que sirva como centro de distribuição, atendimento, coleta e etc \citep{Daskin1995}.

    As aplicações deste tipo de problema são geralmente usadas pelo poder público ou pela iniciativa privada e possuem objetivos diversos como por exemplo reduzir custo operacional ou melhorar a qualidade de atendimento aos clientes, empresas ou a população. Alguns exemplos de aplicações são: definir onde serão implantados centros de distribuição de mercadorias; estruturas públicas como escolas, hospitais, represas e etc; instalação de fábricas levando em consideração fonte de matéria-prima e mercado consumidor; instalação de antenas de telecomunicação (ver Figura~\ref{fig:mapa}) considerando a cobertura do serviço entre outras \citep{Lorena2002, Rosario2002}.
    
    \begin{figure}[htb]
        \centering
        \includegraphics[width=0.9\textwidth]{figs/mapa.png}
        \caption{Representação visual do problema das $p$-medianas.}
        \label{fig:mapa}
    \end{figure}

    Esta classe de problemas está dividida em 2 subproblemas: o problema dos centros e o problema das medianas. No problema dos centros, o objetivo é minimizar a distância até o ponto mais crítico, ou seja, minimizar a maior distância a ser percorrida. No problema das medianas, o objetivo é minimizar a soma de todas as distâncias de cada ponto de demanda à facilidade mais próxima. Este problema consiste em determinar pontos de localização cujas demandas precisam se deslocar aos pontos de localização de facilidades para utilizarem seus serviços \citep{Rosario2002}. 
    
    \begin{defi}\label{defi:pmediana}
        Seja $G(V, A)$ um grafo não orientado onde $V$ é o conjunto dos vértices do grafo e $A$ é o conjunto das arestas. Deve-se encontrar um conjunto de vértices $V_p \subset V$ com cardinalidade $p$, tal que a soma das distâncias de cada vértice pertence a $V$ até seu vértice mais próximo em $V_p$ seja a mínima possível.
    \end{defi}

    De forma resumida, o problema das $p$-medianas tem como objetivo determinar os locais de \emph{p} facilidades (denominadas medianas) em uma rede de \emph{n} nós, de modo a minimizar a soma das distâncias entre cada nó de demanda e a mediana mais próxima. O problema das $p$-medianas é reconhecido como sendo $NP$-Difícil, ou seja, é provável que não exista um algoritmo capaz de encontrar uma solução ótima e prová-la em um número de passos polinomial dado o tamanho de entrada \citep{Garey1979,Senne2003}.

    No caso do problema de localização $p$-mediana, pode-se utilizar programação matemática inteira. Segundo \cite{Christofides1975}, tem se:
    
    {\allowdisplaybreaks\begin{align}
        v(P) =  \min \sum_{i \in N}{ \sum_{j \in N}{ d_{ij} x_{ij} } }
    \end{align}}
    
    \noindent sujeito a:
    
    {\allowdisplaybreaks\begin{align}
        & \sum_{i \in N}{ x_{ij} = 1};~~j \in N\\
        & \sum_{i \in N}{ x_{ii} = p}\\
        & x_{ij} \le x_{ii};~~i, j \in N\\
        & x_{ij} \in \{0, 1\};~~i, j \in N
    \end{align}}
    
    \noindent onde:
    
    \begin{itemize}
        \item $P$ é a instância;
        \item $N = \{1, ..., n\}$;
        \item $[d_{ij}]_{n\times n}$ é uma matriz simétrica de custos, com $d_{ii} = 0,~\forall i \in N$;
        \item $[x_{ij}]_{n\times n}$ é uma matriz de alocação, com $x_{ij} = 1$ se o nó $j$ é alocado à mediana i, e $x_{ij} = 0$, caso contrário; $x_{ii} = 1$ se o nó $i$ é uma mediana e $x_{ii} = 0$, caso contrário;
        \item $p$ é o número de medianas;
        \item $n$ é o número de nós.
    \end{itemize}
    

%%%%%%%%%%%%%%%%%%%%%%%%%% Metodologias %%%%%%%%%%%%%%%%%%%%%%%%%%


\section{Metodologias para a Solução do Problema}
    \label{sec:metodologia}
    
    \subsection{Método Branch-and-Bound}
    \label{sec:met_simplex}
    
    O algoritmo \emph{Branch-and-Bound} é um dos procedimentos mais utilizados na resolução de problemas de programação linear inteira. A ideia geral é dividir o conjunto de soluções viáveis em subconjuntos sem interseções entre si, calculando os limites superior e inferior para cada subconjunto, e eliminar certos subconjuntos de acordo com algumas restrições \cite{Lachtermacher2009}.
       

    \subsection{Programação Paralela}
    \label{sec:met_paralela}
    
        Computação paralela é um paradigma onde várias instruções são executadas ao mesmo tempo, de forma concorrente. A ideia central por traz desta abordagem é que grandes problemas podem ser divididos em vários pequenos problemas, e como isso, diminuir o tempo de execução de aplicações \citep{Almasi1989}.
        
        Algoritmos paralelos podem rodar sobre diferentes arquiteturas, como CPU multinúcleo, clusters, sistemas em grid, GPU entre outros. Este trabalho tomará como base a computação em GPU. \emph{Graphics processing unit} (GPU) é um tipo de microprocessador especializado em processamento gráfico. Entretanto, atualmente as GPUs estão sendo largamente usadas para acelerar aplicações científicas, aplicações em engenharia e em outros tipos de aplicações que necessitem de grande quantidade de processamento e onde a paralelização seja viável \citep{GPU2015}.
        
        \emph{Compute Unified Device Architecture} (ou CUDA) é uma plataforma de computação paralela que tira proveito de GPUs para resolver problemas computacionais mais rapidamente. A plataforma CUDA é compatível com GPUs da NVidia e as aplicações nela executadas podem ser escritas nas linguagens: \emph{C}, \emph{C++} e \emph{FORTRAN} \citep{CUDA2015}.
    https://www.sharelatex.com/project

%%%%%%%%%%%%%%%%%%%%%%%%%% Proposta %%%%%%%%%%%%%%%%%%%%%%%%%%


\section{Proposta}
    \label{sec:proposta}
    
    Este trabalho tem como objetivo investigar diferentes formas de resolver o problema das $p$-medianas e com isso, verificar qual das técnicas terá o melhor desempenho. Para isso, duas abordagens serão adotadas, a primeira é através do método \emph{Branch-and-Bound}, que foi discutido na Seção \ref{sec:met_simplex}. A segunda abordagem diz respeito a programação paralela, como visto na Seção \ref{sec:met_paralela}. 
    
    Pretende-se implementar as duas estratégias e gerar diferentes instâncias do problema a fim de verificar o desempenho de ambos as solução em diversos contextos. O estudo segue uma hipótese em que o método \emph{Branch-and-Bound} terá um desempenho melhor que o método paralelo para instâncias pequenas e o desempenho do método paralelo irá superar o \emph{Branch-and-Bound} para instâncias grandes. O motivo desta hipótese é a quantidade instruções utilizadas para migrar as variáveis para a \emph{GPU} e a retorná-las para o \emph{host}.  
    
    Além desta duas abordagens, também será implementado um método serial que testará todas as soluções possíveis. Este método servirá como base de comparação entre os outros dois.
    
    
%%%%%%%%%%%%%%%%%%%%%%%%%% Experimentos Computacionais %%%%%%%%%%%%%%%%%%%%%%%%%%


\section{Experimentos Computacionais}
    \label{sec:exp_comp}
    
    Primeiramente foi feito um \emph{script} para gerar o grafo das instancias do problema. Convencionou-se que os grafos seriam representados por intermédio de matrizes de adjacência, e que cada elemento desta matriz estaria em uma linha do arquivo de entrada. 
    
    As instâncias do problema das $p$-medianas usadas no estudo estão apresentadas na Tabela \ref{tab:inst_prob}, onde cada linha é uma instâncias e os valores das colunas são os números de nós, números de medianas, menor custo e os melhores nós de facilidades. 

    \begin{table}[h]
    \centering
    \begin{tabular}{cccr}
    \hline
    \multicolumn{1}{l}{\textbf{\begin{tabular}[c]{@{}l@{}}Número\\ de Nós\end{tabular}}} & \multicolumn{1}{l}{\textbf{\begin{tabular}[c]{@{}l@{}}Número de \\Medianas\end{tabular}}} & \multicolumn{1}{l}{\textbf{Menor Custo}} & \multicolumn{1}{l}{\textbf{Facilidades}} \\ \hline
    20                                                                                 & 3                                                                                        & 1117                                     & 07 08 12                                 \\
    35                                                                                 & 3                                                                                        & 2058                                     & 01 10 34                                 \\
    20                                                                                 & 5                                                                                        & 829                                      & 02 07 08 12 15                           \\
    50                                                                                 & 3                                                                                        & 2375                                     & 07 27 29                                 \\
    35                                                                                 & 5                                                                                        & 1593                                     & 01 03 10 17 34                           \\
    50                                                                                 & 5                                                                                        & 1870                                     & 01 27 28 29 32                           \\ \hline
    \end{tabular}
    \caption{Instâncias do problema.}
    \label{tab:inst_prob}
    \end{table}

    Os experimentos forma realizados por três diferentes métodos, todos implementados na linguagem de programação \emph{C++}. O primeiro método foi o desenvolvimento de um algoritmo serial que verificasse todas as soluções possíveis para cada instância do problema. Este método adota as combinações de $p$ facilidades como soluções a serem verificadas, ou seja, para uma instância de $n > 4$ vértices e $p = 3$ facilidades, as primeira solução são os vértices ${0, 1 , 2}$, a segunda solução é ${0, 1, 3}$ e assim sucessivamente. Sendo que a quantidade de combinações de medianas possíveis pode ser obtida com o cálculo da Equação \ref{eq:comb_med}. 
    
 
    
    {\allowdisplaybreaks\begin{align}
        Q =  {n \choose p} = \frac{n!}{p! \times (n - p)!}
        \label{eq:comb_med}
    \end{align}}
    
    \noindent onde n é o número de nós e p é o número de medianas.
    
    Neste cenário (e no método paralelo também), a alocação dos nós de clientes às facilidade é feita por meio do cálculo de distâncias entre o cliente e todas as facilidades da solução corrente, desta forma, o cliente é alocado à facilidade que estiver mais próxima em termo de custo.  
    
    O segundo método foi implementado tomando como base o algoritmo do primeiro método e paralelizando o que foi possível de ser paralelizado. Embora o código fonte deste método esteja com a extensão ".cu"~(extensão padrão do CUDA), ele foi implementado na linguagem \emph{C++}.
    
    O Algoritmo \ref{alg:vns_basico} descreve como o método paralelo foi implementado. Inicialmente os valores de \emph{N} (número de nós), \emph{P} (número da medianas) e \emph{Q} (Quantidade de combinações de \emph{P} medianas obtidas com a Equação \ref{eq:comb_med}) são definidos nas linhas 1, 2 e 3. A matriz \emph{mCombMed} recebe todas as possíveis combinações de facilidades na linha 4, onde cada linha é uma combinação e cada coluna é um elemento (nó de facilidade). Já na linha 5 é declarado um array (\emph{acumuladoSolucoes}) para o acumulado das combinações de medinas. A linha 6 declara o procedimento \emph{calAcumSolThread} e a linha 20 declara o \emph{calMenorDistThread}. Na linha 31 são iniciadas as thread para executar o procedimento \emph{calAcumSolThread}.
    
    O procedimento \emph{calAcumSolThread} é realizado em paralelo. Este procedimento recebe um índice \emph{i} que é referente a uma posição no array \emph{acumuladoSolucoes}. Na linha 7 é declarado o array de distâncias \emph{dist}, já na linha 8 o array \emph{solAtual} recebe os nós de facilidades da solução que está sendo analisada. O cálculo das distâncias entre os clientes e as facilidades é realizado no laço da linha 9 até a linha 15. A linha 10 verifica se o nó \emph{j} é uma facilidade, e no caso de não ser uma facilidade, o método \emph{ calMenorDistThread} é chamado. A linha 16 espera todas as thread do método \emph{ calMenorDistThread} serem executadas para podem passar o fluxo de execução para a linha 17. Na linha 17, todos os valores de distâncias calculados e escritos no array \emph{dist} são somados, e o valor do acumulado das distâncias entre os cliente e às facilidades mais próximas escritos na posição \emph{i} do array \emph{acumuladoSolucoes}.
    

\begin{algorithm}[h]
    \caption{Algoritmo Paralelo.}
    \label{alg:vns_basico}
    \begin{algorithmic}[1]
    \footnotesize
    
        \State{N $\gets$ número de nós}
        \State{P $\gets$ número de medianas}
        \State{Q $\gets$ quantidade de combinação de 'P' medianas}
        
        \State{mCombMed[Q][P] $\gets$ gerarCombMed(N, P)}
        \State{acumuladoSolucoes[Q]}
      
        \Procedure{calAcumSolThread}{$i$}
          
            \State{dist[N]}
          
            \State{solAtual[P] $\gets$ mCombMed[i][P]}
            
            \For{(j in N)}
            
                \If{(j não esta em solAtual[P])}
                
                    \State{calMenorDistThread(j, dist[N], solAtual[P])}
                    
                \Else
                
                    \State{dist[j] $\gets$ 0}
                
                \EndIf
            
            \EndFor
            
            \State{<SincronizarThread>}
            \State{somaAcm $\gets$ somaElementos(dist[N])}
            \State{acumuladoSolucoes[i] $\gets$ somaAcm}

        \EndProcedure      
        
        \Procedure{calMenorDistThread}{$j$, $dist[N]$, $solAtual[P]$}
          
            \State{menorDist $\gets$ MAX\_NUM}
            
            \For{(k in P)}
            
                \State{d $\gets$ distEntreNos(j, k)}
        
                \If{(d $<$ menorDist)}
                
                    \State{menorDist $\gets$ d}
                
                \EndIf
            
            \EndFor
            
            \State{dist[j] $\gets$ menorDist}

        \EndProcedure
        
            \For{(j in Q)}
            
                \State{calAcumSolThread(i)}
            
            \EndFor
            
            \State{<SincronizarThread>}
            \State{idx $\gets$ indexMenorElemento(acumuladoSolucoes[Q])}
            \State{melhoresMedianas[P] $\gets$ mCombMed[idx][P]}
        
    \end{algorithmic}
\end{algorithm}
    
    Como mencionado acima, o procedimento \emph{calMenorDistThread} é chamado por todas as thread de processamento \emph{calAcumSolThread} e também é paralelo. Este procedimento é responsável por determinar a menor distância entre um nó cliente e as facilidades. Na linha 21, a variável \emph{menorDist} recebe o maior valor possível de um numero na linguagem C++. O laço da linha 22 varia entre os vértices de mediana, e a distância entre estes e o nó cliente (\emph{j}) é obtida na linha 23. No caso da distância entre o cliente \emph{j} e a facilidade \emph{k} for menor que a menor distâncias atual, então a variável \emph{menorDist} é atualizada. Por fim, a linha 28 escreve a menor distância encontrada na posição \emph{j} do array \emph{dist}.
    
    Depois destas tarefas, a linha 33 espera a conclusão das execuções de todas as threads do procedimento \emph{calAcumSolThread}, e com isso, o fluxo do algoritmo volta a ser serial. A linha 34 procura o menor elemento no array \emph{acumuladoSolucoes} e retorna o índice deste. Na linha 35, a solução com as melhores facilidades, e consequentemente o menor custo de deslocamento entre os clientes e às facilidades, é escrita no array \emph{melhoresMedianas} e obtida através da matriz \emph{mCombMed} com o índice encontrado na linha 34.
    
    No terceiro e último método foi feita uma modelagem de programação linear, e posteriormente foi codificado um algoritmo para, através do \emph{Cplex}, solucionar as instâncias do problema pelo método \emph{Branch-and-Bound}. Para esse método também foi usada a linguagem de programação \emph{C++}.

    \begin{table}[h]
    \centering
    \begin{tabular}{ccrrrr}
    \hline
    \multicolumn{1}{l}{\textbf{\begin{tabular}[c]{@{}l@{}}Num. \\ de Nós\end{tabular}}} & \multicolumn{1}{l}{\textbf{\begin{tabular}[c]{@{}l@{}}Num. de \\ Facilidades\end{tabular}}} & \multicolumn{1}{l}{\textbf{\begin{tabular}[c]{@{}l@{}}Num. de Soluções\\ Possíveis\end{tabular}}} & \textbf{\begin{tabular}[c]{@{}l@{}}Tempo (sec)\\ Serial\end{tabular}} & \multicolumn{1}{l}{\textbf{\begin{tabular}[c]{@{}l@{}}Tempo (sec)\\ GPU\end{tabular}}} & \multicolumn{1}{l}{\textbf{\begin{tabular}[c]{@{}l@{}}Tempo (sec)\\ Cplex\end{tabular}}} \\ \hline
    20                                                                                  & 3                                                                                           & 1.140                                                                                             & 0,625                                                                      & 0,100                                                                                  & 0,044                                                                                    \\
    35                                                                                  & 3                                                                                           & 6.545                                                                                             & 21,185                                                                      & 0,640                                                                                  & 0,135                                                                                    \\
    20                                                                                  & 5                                                                                           & 15.504                                                                                            & 16,120                                                                      & 1,620                                                                                  & 0,044                                                                                    \\
    50                                                                                  & 3                                                                                           & 19.600                                                                                            & 155,972                                                                      & 1,900                                                                                  & 0,316                                                                                    \\
    35                                                                                  & 5                                                                                           & 324.632                                                                                           & 1314,817                                                                      & 37,250                                                                                 & 0,147                                                                                    \\
    50                                                                                  & 5                                                                                           & 2.118.760                                                                                         & 7200,000*                                                                    & 231,85                                                                                 & 0,320                                                                                    \\ \hline
    \end{tabular}
    \caption{Comparativo do tempo de execução dos métodos implementados.}
    \label{tab:resul-tempo}
    \end{table}

    A Tabela \ref{tab:resul-tempo} mostra os resultados dos três métodos avaliados. Nela é possível observar que os tempos de execução dos métodos serial e paralelo estão crescendo conforme o aumento da quantidade de soluções possíveis para cada instância, sendo que o crescimento do tempo do método paralelo é bem menor que o do método serial. Um detalhe nesta tabela é que o tempo do método serial da última instância avaliada é de 7200 segundos e possui um (*), isso se deu porque o tempo de execução passou de 2 horas, e como esse já era um tempo bastante elevado em relação aos demais (ver Figura \ref{fig:graf_tempo}), então foi decidido por abortar a execução deste.

    \begin{figure}[h]
        \centering
        \includegraphics[width=0.9\textwidth]{figs/graf_tempo_exec.eps}
        \caption{Tempo de execução dos métodos.}
        \label{fig:graf_tempo}
    \end{figure}
    
    Os resultados também mostram que, embora o método paralelo tenha se saído melhor que o método serial, o mesmo não aconteceu com a implementação do método Branch-and-Bound. Possivelmente este fenômeno ocorreu porque o tempo de execução do Cplex não está necessariamente relacionada à quantidade de soluções possíveis. Isso é mostrado na última coluna da Tabela \ref{tab:resul-tempo}, onde o tempo não está, obrigatoriamente, variando de forma crescente. 
    
    
%%%%%%%%%%%%%%%%%%%%%%%%%% Considerações Finais %%%%%%%%%%%%%%%%%%%%%%%%%%


\section{Considerações Finais}
    \label{sec:conclusão}

    Este trabalho teve como objetivo avaliar diferentes abordagens para a solução do problemas das $p$-medianas. Entre os três métodos adotados, o que usava o Cplex obteve o melhor desempenho. O método paralelo usando a plataforma CUDA teve um desempenho mediano, pois este tratava de todas a soluções possíveis para as instâncias do problema, o que não acontecia com a implementação usando o Cplex.
    
    Como trabalhos futuros pretende-se implementar e avaliar métodos alternativos para a solução do problema das $p$-medianas. Tais métodos também poderão ser implementados de forma paralela com a plataforma CUDA ou ainda feitos para rodar sobre clusters. Entretanto, serão considerandos métodos que não explorem todas as soluções possíveis, como é o caso do próprio Branch-and-Bound ou de meta-heurísticas.

\bibliography{refs}
\end{document}
