\documentclass[twoside,english,brazilian]{UNISINOSmonografia}
\usepackage[utf8]{inputenc} % charset do texto (utf8, latin1, etc.)
\usepackage[T1]{fontenc}    % encoding da fonte (afeta a sep. de sílabas)
\usepackage{bibentry}       % para inserir refs. bib. no meio do texto
\usepackage[alf]{abntex2cite}

%=======================================================================
% Dados gerais sobre o trabalho (mantidos para capa/folha de rosto).
%=======================================================================
\autor{Figueiredo Carolino}{Gabriel}
\titulo{Sistema de Irrigação Automatizado para Plantas Domésticas baseado em IoT (Arduino + FlutterFlow)}
\subtitulo{Soluções Inovadoras para a Irrigação Automatizada em Casa}
\orientador[Dr.]{Lacerda}{Guilherme}

\unidade{Unidade Acadêmica de Graduação}
\curso{Curso de Bacharelado em Ciência da Computação}
\natureza{%
Monografia apresentada como requisito parcial para obtenção do título de Bacharel em Ciência da Computação, pelo Curso de Ciência da Computação da Universidade do Vale do Rio dos Sinos (UNISINOS)
}
\local{Porto Alegre}
\ano{2025}

% Palavras-chave em PT para metadados (mantidas)
\palavrachave{brazilian}{automação residencial}
\palavrachave{brazilian}{IoT}
\palavrachave{brazilian}{irrigação automatizada}
\palavrachave{brazilian}{Arduino}
\palavrachave{brazilian}{FlutterFlow}

%=======================================================================
% Início do documento.
%=======================================================================
\begin{document}
\capa
\folhaderosto

%=======================================================================
% Página inicial do artigo no padrão do exemplo da UNISINOS
% (título PT/EN, autoria com notas de rodapé, resumo/abstract e palavras-chave).
%=======================================================================
\clearpage
\thispagestyle{plain}
\begin{center}
\MakeUppercase{TÍTULO EM PORTUGUÊS: Sistema de Irrigação Automatizado para Plantas Domésticas baseado em IoT (Arduino + FlutterFlow)}\\[0.75em]
\MakeUppercase{TÍTULO EM OUTRO IDIOMA: Automated Irrigation System for Household Plants based on IoT (Arduino + FlutterFlow)}\\[1.5em]

Gabriel Figueiredo Carolino\footnote{Bacharelando em Ciência da Computação (UNISINOS). E-mail: \texttt{gabrielcarolino962@gmail.com}.} \\
Orientador: Prof. Dr. Guilherme Lacerda\footnote{UNISINOS. E-mail: \texttt{guilhermeslacerda@gmail.com}.}
\end{center}

\bigskip

\noindent\textbf{Resumo:} Este artigo apresenta o desenvolvimento e a avaliação de um sistema de irrigação automatizado para ambientes residenciais, baseado em tecnologias de Internet das Coisas (IoT). A solução integra sensores de umidade do solo a um microcontrolador Arduino, comunicação Wi-Fi e um backend em Firebase, além de uma interface desenvolvida no FlutterFlow para monitoramento e controle remoto. O sistema ajusta a irrigação conforme as condições do solo, visando reduzir desperdício de água e facilitar o cuidado com plantas domésticas. São descritos a arquitetura proposta, os materiais e métodos de implementação e um protocolo experimental para quantificar economia de água, estabilidade da umidade do solo e responsividade do sistema.

\medskip
\noindent\textbf{Palavras-chave:} automação residencial; Internet das Coisas; irrigação automatizada; Arduino; FlutterFlow.

\medskip
\noindent\textbf{Abstract:} This paper presents the development and evaluation of an automated irrigation system for residential environments, leveraging Internet of Things (IoT) technologies. The solution integrates soil moisture sensors with an Arduino microcontroller, Wi-Fi communication, and a Firebase backend, along with a FlutterFlow-based interface for remote monitoring and control. The system adapts irrigation to soil conditions to reduce water waste and simplify household plant care. We describe the proposed architecture, materials and methods, and an experimental protocol to quantify water savings, soil moisture stability, and system responsiveness.

\medskip
\noindent\textbf{Key-words:} home automation; Internet of Things; automated irrigation; Arduino; FlutterFlow.

%=======================================================================
% 1 INTRODUÇÃO (no tom e estrutura de artigo).
%=======================================================================
\chapter{Introdução}

A automação residencial aliada à Internet das Coisas (IoT) tem viabilizado soluções que otimizam tarefas cotidianas e elevam a eficiência no uso de recursos. No contexto do cultivo de plantas em ambientes domésticos, a irrigação adequada é um desafio recorrente devido à variação de necessidades entre espécies, à falta de tempo dos usuários e à dificuldade de calibrar volume e frequência de água. Práticas inadequadas — irrigação excessiva ou insuficiente — comprometem a saúde das plantas e ampliam o desperdício de água.

Este trabalho aborda esse problema por meio do desenvolvimento de um sistema de irrigação automatizado de baixo custo, capaz de monitorar continuamente a umidade do solo e acionar a irrigação conforme limiares configuráveis por planta. A solução integra sensores de umidade a um Arduino com conectividade Wi-Fi, um backend em Firebase para troca de dados em tempo real e um aplicativo/interface web construído no FlutterFlow para monitoramento e controle remoto.

\medskip
\noindent\textbf{Objetivo.} Desenvolver e avaliar um sistema IoT de irrigação residencial que:
\begin{itemize}
    \item monitore, em tempo real, a umidade do solo e variáveis ambientais relevantes;
    \item ajuste automaticamente a irrigação conforme parâmetros específicos por planta;
    \item disponibilize interface intuitiva com histórico de dados e comandos remotos.
\end{itemize}

\noindent\textbf{Contribuições.} Este artigo apresenta:
\begin{itemize}
    \item uma arquitetura IoT de baixo custo (Arduino + ESP8266 + Firebase + FlutterFlow) replicável em ambiente doméstico;
    \item integração em tempo real com armazenamento histórico e controle remoto;
    \item um protocolo experimental reprodutível para mensurar economia de água, estabilidade da umidade do solo e responsividade;
    \item discussão de limitações e um roteiro para evolução com aprendizado de máquina e dados meteorológicos.
\end{itemize}

\noindent\textbf{Organização do artigo.} A Seção 2 apresenta a fundamentação teórica e os trabalhos relacionados; a Seção 3 descreve materiais e métodos; a Seção 4 detalha o sistema proposto; a Seção 5 apresenta experimentos e resultados; a Seção 6 discute os achados, limitações e ameaças à validade; a Seção 7 traz a conclusão e trabalhos futuros.

%=======================================================================
% 2 FUNDAMENTAÇÃO TEÓRICA (unifica referencial + estado da arte)
%=======================================================================
\chapter{Fundamentação Teórica}

\section{Automação Residencial e Internet das Coisas (IoT)}
A automação residencial, ou domótica, aplica tecnologias de controle e monitoramento para tornar residências mais eficientes, confortáveis e seguras. A IoT atua como elemento habilitador ao conectar sensores, atuadores e serviços em rede, permitindo coleta de dados e tomada de decisão em tempo real \cite{silva2019}. Em cenários domésticos, a integração de dispositivos inteligentes possibilita adaptar o funcionamento do ambiente às necessidades dos usuários, com impacto direto em eficiência energética e sustentabilidade.

\section{Sistemas Automatizados de Irrigação}
Soluções de irrigação automatizada empregam sensores de umidade do solo, temperatura e, em alguns casos, luminosidade e pluviometria, para ajustar o fornecimento de água conforme a demanda da planta e as condições ambientais \cite{fernandes2021}. Estudos relatam benefícios como redução de desperdício hídrico, maior consistência na manutenção da umidade ideal do substrato e conveniência aos usuários \cite{pereira2021}. A viabilidade desses sistemas em ambientes residenciais cresce com a disponibilidade de componentes de baixo custo e plataformas acessíveis.

\section{Integração de Sensores, Atuadores e Plataformas}
Arquiteturas baseadas em microcontroladores (por exemplo, Arduino) com módulos de conectividade (como ESP8266) viabilizam a aquisição de dados de sensores e o acionamento de bombas ou válvulas \cite{almeida2020}. A integração com plataformas em nuvem e bancos de dados em tempo real (e.g., Firebase) facilita o armazenamento histórico, a sincronização de estados e o envio de comandos remotos por meio de aplicativos móveis ou interfaces web. Essa pilha tecnológica favorece escalabilidade funcional e manutenção simplificada.

\section{Sustentabilidade e Eficiência Hídrica}
No contexto de sustentabilidade, a irrigação inteligente contribui para uso racional da água, ao evitar tanto a subirrigação quanto a superirrigação. A literatura indica potencial de redução significativa do consumo quando sensores e algoritmos de controle ajustam a irrigação às condições reais do solo. Em ambientes domésticos, além do impacto ambiental positivo, há ganhos práticos na saúde das plantas e na previsibilidade de cuidados, especialmente para usuários não especialistas.

\section{Trabalhos Relacionados}
Diversos trabalhos exploram a interseção entre IoT, automação residencial e irrigação. \citeonline{volkmann2022} descrevem uma solução de baixo custo que integra sensores de umidade, temperatura e luminosidade com atuadores hidráulicos, comunicação MQTT e interface móvel, demonstrando eficiência e redução de desperdício após testes em jardim residencial. \citeonline{medeiros2021} apresentam um sistema voltado a plantas domésticas, com monitoramento contínuo e interface web para configuração remota, destacando economicidade de água e manutenção do solo em condições ideais. Em revisão, \citeonline{lima2019} analisam dezenas de estudos focados em automação da irrigação, evidenciando a tendência por soluções acessíveis e a integração crescente de IoT e controle automatizado para irrigação de precisão.

\subsection{Diferenciais e Lacunas}
Em relação aos trabalhos anteriores, o presente sistema enfatiza:
\begin{itemize}
    \item simplicidade de montagem e custo reduzido, priorizando componentes amplamente disponíveis (Arduino Mini, sensores de umidade, ESP8266);
    \item backend em tempo real com Firebase e interface desenvolvida no FlutterFlow, reduzindo barreiras de desenvolvimento front-end;
    \item visão de evolução para aprendizado de máquina e integração com dados meteorológicos, superando esquemas de controle baseados apenas em limiares fixos.
\end{itemize}
Persistem lacunas importantes, como: (i) padronização de protocolos de calibração de sensores para diferentes tipos de solo; (ii) avaliação quantitativa de economia de água em cenários residenciais variados; e (iii) estudo sistemático de confiabilidade e latência fim a fim em condições de conectividade doméstica.

%=======================================================================
% (Próximas partes: 3 Materiais e Métodos; 4 Sistema Proposto; 5 Resultados; etc.)
%=======================================================================

\chapter{Materiais e Métodos}

\section{Visão geral da arquitetura}
O sistema segue uma arquitetura em três camadas: (i) coleta de dados por sensores de umidade do solo conectados a um microcontrolador (Arduino) e a atuadores (bomba ou válvula) para irrigação, (ii) processamento e sincronização em tempo real via backend em nuvem (Firebase) e (iii) interface de interação (aplicativo/WEB em FlutterFlow) para monitoramento e controle remoto. Essa organização favorece modularidade, manutenção e reuso.

\section{Materiais}
\subsection{Hardware}
Os componentes utilizados (ou equivalentes compatíveis) são:
\begin{itemize}
    \item \textbf{Microcontrolador:} Arduino Mini (3{,}3\,V ou 5\,V), responsável pela leitura dos sensores e acionamento dos atuadores.
    \item \textbf{Conectividade:} Módulo Wi-Fi ESP8266 (p.ex., ESP-01 ou NodeMCU) para comunicação com o backend em nuvem.
    \item \textbf{Sensores de umidade do solo:} Preferencialmente \emph{capacitivos}, pela maior resistência à corrosão e estabilidade frente aos \emph{resistivos}.
    \item \textbf{Atuadores:} Bomba peristáltica ou submersa de baixo porte, ou válvula solenóide (12\,V) para controle do fluxo de água.
    \item \textbf{Acionamento de potência:} Módulo relé.
    \item \textbf{Fonte de alimentação:} 5\,V para eletrônica e 12\,V (se necessário) para válvula/bomba, com aterramento comum e proteção (fusível).
    \item \textbf{Demais itens:} Mangueiras, reservatório, conectores, protoboard/PCB, cabos e caixa para acomodação segura (proteção contra respingos).
\end{itemize}

\subsection{Software}
\begin{itemize}
    \item \textbf{Firmware (Arduino):} Implementa a leitura analógica dos sensores (com filtragem), lógica de controle baseada em limiares de umidade, histerese e temporizações de segurança; realiza a comunicação com o backend (via Wi-Fi).
    \item \textbf{Backend:} Firebase (Realtime Database ou Firestore) para armazenamento de leituras, histórico e comandos; regras de segurança para autenticação/autorização.
    \item \textbf{Frontend:} Aplicativo/Interface web construída no FlutterFlow, com telas para status em tempo real, históricos, configuração de limiares e acionamento manual \cite{flutterflow}.
\end{itemize}

\section{Arquitetura lógica e fluxos}
O Arduino amostra os sensores em janela temporal (p.ex., a cada 15--60\,s), aplica filtragem e publica as leituras no backend. A lógica de irrigação ocorre localmente (no firmware), conferindo autonomia mesmo com intermitências de rede: quando a umidade medida cruza abaixo do limiar inferior, inicia-se a irrigação até alcançar o limiar superior (histerese), respeitando tempos máximos contínuos e intervalos mínimos entre ciclos para evitar saturação do solo. O aplicativo consome os dados, exibe históricos e envia comandos de configuração/override. Essa abordagem híbrida (controle local + supervisão remota) é recomendada em sistemas IoT residenciais.

\section{Calibração dos sensores de umidade}
Para reduzir variabilidade entre sondas e tipos de solo, adota-se calibração de dois pontos:
\begin{enumerate}
    \item \textbf{Ponto seco (0\%):} Medir a leitura analógica (\emph{raw\_dry}) com o sensor em solo completamente seco.
    \item \textbf{Ponto saturado (100\%):} Medir (\emph{raw\_wet}) após saturar o solo com água e aguardar a estabilização (sem lâmina livre).
\end{enumerate}
A conversão para porcentagem utiliza:
\[
\mathrm{Umidade}(\%) = \frac{\mathrm{raw\_wet} - \mathrm{raw\_atual}}{\mathrm{raw\_wet} - \mathrm{raw\_dry}} \times 100
\]
Aplica-se \emph{clamping} para manter o resultado em [0,\,100]. Recomenda-se:
\begin{itemize}
    \item média de $n\geq 5$ leituras por ponto; 
    \item filtragem (mediana + média móvel) para reduzir ruído;
    \item calibração por tipo de solo (universal, argiloso, arenoso), pois a constante dielétrica afeta a leitura.
\end{itemize}

\section{Parâmetros de controle (limiares e histerese)}
Define-se uma faixa de umidade-alvo por planta/solo (p.ex., 35--45\% para espécie A em solo universal), ajustável pelo usuário. Proteções:
\begin{itemize}
    \item tempo máximo por ciclo (p.ex., 20--60\,s);
    \item intervalo mínimo entre ciclos (p.ex., 10--30\,min);
\end{itemize}

\subsubsection*{Nota prática sobre limiares, histerese e tempos de proteção}
A \textbf{umidade do solo (\%)} é \emph{medida} pelo sensor (após calibração). Já a \textbf{faixa de umidade-alvo} (p.ex., 35--45\%), os \textbf{limiares} (inferior/superior), a \textbf{histerese}, o \textbf{tempo máximo por ciclo}, o \textbf{intervalo mínimo entre ciclos} e a \textbf{janela de silêncio} são \emph{parâmetros de controle} definidos pelo usuário para dar estabilidade e segurança ao sistema.

\begin{itemize}
    \item \textbf{Histerese:} usar dois limiares evita liga/desliga rápido próximo ao limite.
    \item \textbf{Tempo máximo por ciclo:} limita a irrigação contínua (protege contra erro de sensor ou vazão alta).
    \item \textbf{Intervalo mínimo:} garante tempo para infiltração antes de nova irrigação.
\end{itemize}

\noindent\textbf{Exemplo:} faixa 35--45\%, $t_{\max}=30$ s, intervalo mínimo 15 min. Se a umidade cair a 34\%, a bomba liga e desliga ao atingir 45\% ou 30 s, o que vier primeiro; depois aguarda 15 min para qualquer novo ciclo.

\section{Protocolo experimental}
Para avaliar a solução em ambiente doméstico, propõe-se:
\begin{itemize}
    \item \textbf{Cenário e duração:} Ambiente interno, 14 dias.
    \item \textbf{Amostras:} Duas espécies ornamentais comuns (p.ex., Espécie A e B), 3 vasos por espécie e por grupo (controle e automatizado): total de 12 vasos.
    \item \textbf{Substrato e recipientes:} Solo universal comercial, vasos idênticos (volume e drenagem padronizados).
    \item \textbf{Grupos:} 
        \begin{enumerate}
            \item \emph{Controle manual}: Rega manual diária/padronizada.
            \item \emph{Automatizado IoT}: Controle por limiares com histerese via sistema proposto.
        \end{enumerate}
    \item \textbf{Coleta de dados:} Leituras de umidade a cada 1--5 minutos; registros de acionamento (timestamp, duração); volume de água aplicado medido em recipiente graduado (ou \emph{flow meter}).
    \item \textbf{Condições ambientais:} Registrar temperatura e luminosidade ambiente (se disponível).
\end{itemize}


\section{Métricas e métodos de análise}
As métricas principais são:
\begin{itemize}
    \item \textbf{Economia de água (\%):}
    \[
    \mathrm{Economia}(\%) = \frac{V_{\mathrm{controle}} - V_{\mathrm{auto}}}{V_{\mathrm{controle}}}\times 100
    \]
    onde $V_{\mathrm{controle}}$ e $V_{\mathrm{auto}}$ são os volumes totais aplicados no período.
    \item \textbf{Estabilidade da umidade:} desvio-padrão da umidade e \% do tempo dentro da faixa-alvo (\emph{time-in-target}).
    \item \textbf{Responsividade:} latência fim-a-fim entre cruzamento de limiar e início da irrigação; tempo até retorno à faixa-alvo.
    \item \textbf{Confiabilidade:} disponibilidade do sistema (\%) e taxa de falhas/reconexões de rede.
\end{itemize}

\section{Critérios de exclusão e qualidade dos dados}
Excluir amostras com falhas de sensor evidentes (leituras saturadas constantes), perda prolongada de conectividade ou vazamentos. Registrar eventos anômalos em diário de bordo para rastreabilidade.

\section{Reprodutibilidade}
Publicar lista de materiais, esquema elétrico, versão do firmware e configurações do backend/aplicativo. Sempre que possível, versionar o projeto e fornecer instruções de montagem para facilitar replicação por terceiros, em linha com boas práticas em projetos IoT.
%=======================================================================
% (Próximas partes: 4 Sistema Proposto; 5 Resultados; etc.)
%=======================================================================
\chapter{Sistema Proposto}

\section{Arquitetura física e elétrica}

A solução é organizada em módulos para facilitar montagem, manutenção e evolução:

\begin{itemize}
    \item \textbf{Módulo de sensoriamento:} uma sonda de umidade do solo por vaso (preferência por sensores capacitivos devido à maior estabilidade e menor corrosão), conectada a entradas analógicas do microcontrolador por meio de cabos curtos e blindados quando possível.
    \item \textbf{Módulo de controle:} Arduino Mini encarregado de amostrar sensores, executar a lógica de controle e acionar a irrigação.
    \item \textbf{Módulo de conectividade:} ESP8266 para conexão Wi-Fi e troca de dados com o backend em tempo real.
    \item \textbf{Módulo de acionamento:} relé de estado sólido ou estágio MOSFET com diodo de \emph{flyback} (em cargas indutivas) para acionar bomba submersa/peristáltica ou válvula solenóide.
    \item \textbf{Alimentação:} fonte estabilizada (p.ex., 5\,V para eletrônica; 12\,V para válvula/bomba, se necessário), com aterramento comum, fusível de proteção e organização dos retornos de corrente.
    \item \textbf{Hidráulica:} reservatório, mangueiras e distribuidores dimensionados ao número de vasos; prever retorno anti-sifão e gotejamento uniforme.
\end{itemize}

\noindent\textbf{Considerações de ligação.} Se o Arduino operar a 5\,V e o ESP8266 a 3{,}3\,V, recomenda-se \emph{level shifting} no TX do Arduino para RX do ESP. Alimentar sensores a partir de trilha regulada e estável; evitar compartilhar trilhas de alta corrente com sinais analógicos. Separar fisicamente eletrônica de água (grau de proteção IP na caixa).

\section{Firmware e lógica de controle}

O firmware é estruturado em tarefas cíclicas: aquisição, filtragem, decisão/controle, telemetria e processamento de comandos.


\subsection{Decisão e controle com histerese}
A irrigação é regida por uma máquina de estados simples por planta:
\begin{verbatim}
Estados: IDLE -> IRRIGATING -> LOCKOUT -> IDLE
Transições:
  - IDLE -> IRRIGATING: umidade < limiar_inferior e sem bloqueio
  - IRRIGATING -> LOCKOUT: umidade >= limiar_superior OU tempo_max_atingido
  - LOCKOUT -> IDLE: intervalo_minimo_decorrido
Proteções:
  - tempo_max_irr (p.ex., 20-60 s)
  - intervalo_min_entre_ciclos (p.ex., 10-30 min)
  - janela_silencio (opcional)
\end{verbatim}
A histerese (faixa [limiar\_inf, limiar\_sup]) evita acionamentos intermitentes. Os parâmetros são específicos por planta/vaso e modificáveis via aplicativo.

\subsection{Telemetria, comandos e resiliência}
A cada ciclo, o dispositivo publica no backend: timestamp, umidade (\%), estado da bomba, duração do acionamento (se houver), tensão de alimentação e erros (quando presentes). Comandos de alto nível (e.g., \emph{irrigar agora}, atualização de limiares) são assinados pelo dispositivo. Em falhas de rede, leituras são enfileiradas em \emph{buffer} circular para retentativa; o controle local continua operacional.

\section{Backend e modelo de dados}

Utiliza-se Firebase como camada de dados em tempo real, simplificando sincronização e notificações.

\subsection{Estrutura lógica (Realtime Database)}
Uma organização possível:
\begin{verbatim}
/devices/{deviceId}/telemetry/{ts} : {
  "plantId": "P1",
  "soilMoisture": 41.8,
  "pumpState": false,
  "vSupply": 4.98
}
\end{verbatim}
\begin{verbatim}
/devices/{deviceId}/config/{plantId} : {
  "name": "Espécie A - Vaso 1",
  "moisture": { "low": 35, "high": 45 },
  "timers": { "tMaxIrrSec": 30, "tMinGapMin": 15 },
  "calibration": { "rawDry": 710, "rawWet": 380 }
}
\end{verbatim}
\begin{verbatim}
/devices/{deviceId}/commands : {
  "irrigateNow": { "plantId": "P1", "ttlMs": 15000, "ts": 1712345678 }
}
\end{verbatim}

\section{Interface (FlutterFlow) e telas}

A interface construída no FlutterFlow provê:
\begin{itemize}
    \item \textbf{Dashboard:} cartões por planta com umidade atual, estado (dentro/fora da faixa), botão de \emph{irrigar agora}, e alerta visual para níveis críticos.
    \item \textbf{Detalhe da planta:} gráfico histórico (últimas 24\,h/7\,d), eventos de irrigação (timestamp/duração), status do dispositivo e tensão de alimentação.
    \item \textbf{Configurações:} edição de limiares, histerese (low/high), tempos de proteção, calibração (rawDry/rawWet), e janela de silêncio.
    \item \textbf{Dispositivos:} associação de \emph{deviceId} ao usuário, renomear plantas/vasos, teste de conectividade.
    \item \textbf{Alertas e logs:} lista de eventos, falhas de sensor, perda de rede, e confirmações de comandos.
\end{itemize}
A integração \emph{no-code/low-code} com Firebase acelera o desenvolvimento de formulários e gráficos reativos.

\section{Segurança, privacidade e robustez}

\begin{itemize}
    \item \textbf{Qualidade de dados:} filtros anti-ruído, detecção de valores impossíveis, marcação de leituras suspeitas.
    \item \textbf{Tolerância a falhas:} controle local priorizado; \emph{buffer} de telemetria e retentativa exponencial; \emph{watchdog} no firmware.
\end{itemize}

\section{Custos e replicabilidade}

A seleção de componentes prioriza baixo custo e disponibilidade. Uma configuração típica (Arduino Mini, ESP8266, 2--4 sensores capacitivos, relé/MOSFET e pequena bomba) mantém-se em patamar acessível ao público doméstico, com desempenho comparável ao de soluções acadêmicas de baixo custo. A documentação do projeto (lista de materiais, esquema elétrico, versões de firmware e \emph{export} do app do FlutterFlow) favorece a replicação.

\section{Limitações técnicas e mitigação}

\begin{itemize}
    \item \textbf{Sensores de solo:} variabilidade por lote/solo; mitigar com calibração por vaso e filtragem (mediana + EMA).
    \item \textbf{Conectividade:} Wi-Fi doméstico pode ser instável; mitigar com controle local e retentativas, além de sinalização clara de \emph{offline}.
    \item \textbf{Hidráulica:} desbalanceamento de fluxo entre vasos; mitigar com gotejadores calibrados e testes de vazão.
    \item \textbf{Energia:} dimensionamento da fonte e proteção; mitigar com margens de corrente e fusível.
\end{itemize}

\section{Critérios de aceitação do protótipo}

O sistema é considerado aceitável se: (i) mantém a umidade em faixa-alvo por ao menos 70--85\% do tempo, (ii) reduz o volume de água usado em relação ao controle manual mantendo saúde das plantas, (iii) apresenta latência de resposta abaixo de alguns segundos para comandos e (iv) permanece operacional sob intermitência de rede, em linha com evidências de ganhos de eficiência e praticidade reportadas em trabalhos correlatos.

%=======================================================================
% (Próximas partes: 5 Resultados; etc.)
%=======================================================================
\chapter{Experimentos e Resultados} 

%=======================================================================
% (Próximas partes: 6 Discussão; etc.)
%=======================================================================
\chapter{Discussão}

%=======================================================================
% (Próximas partes: 7 Conclusão; etc.)
%=======================================================================

\chapter{Conclusão e Trabalhos Futuros}

% Exemplo de chaves citadas:
% domingues2013, silva2019, fernandes2021, almeida2020,
% santos2020, pereira2021, volkmann2022, medeiros2021, lima2019

\end{document}