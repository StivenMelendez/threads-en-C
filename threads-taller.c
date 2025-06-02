/*
    Asedio del castillo:
    se requiere una solucion para implementar sistema de asedio a un castillo en un videojuego:
    los castillos son asediados por grupos de infanteria cada grupo esta compuesto por 3000 unidades
    es decir una proporcion de 1:3000, la prueba se realizara sobre un castillo de nivel 4 que ocupa 
    12000 unidades de asedio en total es decir 4 grupos de 3000, un castillo de nivel 4 tiene una guardia
    de 6000 infanterias, esta defiende de manera general el castillo, es decir se enfrentan a los diferentes
    grupos de incursion que asedian el castillo a la vez.
    ATACANTES:
        - la posibilidad de perder el 10% (muertes por atricion) unidades en cada ronda de asedio son de 100%
        - la posibilidad de perder mas del 10% (Causalidades por asedio) unidades por ronda es de 50% luego del calculo anterior
        - la posibilidad de perder un 10% mas de unidades (despistados) es del 10% luego del calculo anterior
        - la posibilidad de avanzar el asedio cada ronda de asedio es de 50%
            + el avance a la caida del castillo aumenta 7%
            + el maximo de avance de caida del castillo es de 100%
    DEFENSORES:
        - la posibilidad de perder 10% de la guarnicion es de 5% (atricion) durante las primeras 30 rondas
        - la posibilidad de perder 10% de la guarnicion en defensa es de 10%.
        - la posibilidad de que se desate una enfermedad en el castillo cada 30 rondas de asedio es de 50%
            + si se desata una enfermedad cada 7 rondas la guarnicion se reduce un 3%
        - la posibilidad de perder 10% de la guarnicion luego de 60 rondas aumenta a 45%
        - la posibilidad de avanzar el asedio cada ronda de asedio es de 50%
            + el avance a la caida del castillo disminuye 7%
            + el maximo de avance de caida del castillo es de -21%
    GENERAL:
        - cada ronda que el asedio sea positivo se estimara si e fuerte puede caer o no.
        - la guarnicion no tiene repuesto dado que el castillo esta bajo asedio
        - los asediantes tienen un limite de reposicion de infanteria
*/