# Repositorio del TP3 - Juan Manuel Carosella Grau

## freeRTOS

### Documentacion de archivos y funciones:

Se muestra una captura de la estructura de los archivos:

![](archivos.jpeg) 

Se pasaran a detallar varios archivos y funciones utilizadas en el codigo.

prvSetupHardware(): Se encuentra en freertos_examples_1_to_9.c y su funcion es inicializar perifericos como los LED y el clock. En hardware se traduce al LED apagado.

DEBUGOUT(): Se encuentra en board_api.h, imprime por consola el string que se le pase como parametro, sirve para, como dice el nombre, debugear. En hardware se traduce a la UART.

xTaskCreate(): Se encuentra en task.c. Crea una tarea segun los parametros pasados. Se explicara mas adelante cada parametro. 

vTaskStartScheduler(): Se encuentra en task.c. Inicia el kernel, se encarga de organizar las tareas temporalmente (asigna el tiempo del CPU). Una desventaja del kernel es que pierde tiempo decidiendo como organizar las tareas.

Board_LED_Set(): Se encuentra en board.c. Simplemente setea el estado del LED segun los parametros pasados.


### Variables:

Las siguentes variables se encuentran en freertos_examples_1_to_9.c.

mainDELAY_LOOP_COUNT es una macro. Define el largo de la demora implementada en el ciclo for.

pcTextForMain es un tipo const char *. Es una string que guarda el nombre del ejemplo.

vTask es un tipo prototipo de la tarea.

ul es un tipo volatile unsigned long el cual es el valor utilizado para implementar la demora.


## Ejemplos 1 al 9
### Ejemplo 1

En este ejemplo, se tienen dos tareas (1 y 2) con igual prioridad. Se configuran de la siguiente manera:

![](C:\Users\camis\Desktop\Camila\Fiuba\SistemasEmbebidos\TP3\Imagenes\ex1_1.PNG)

Se setea un puntero que apunta a la función que implementa la tarea, se le asigna un nombre a la tarea, para facilitar el debug y se configuran otros parametros, como por ejemplo, la prioridad, que en este caso, y en el de la tarea 2 de igual forma, es 1.


La tarea 1 apaga el LED3 y la tarea 2 lo prende. Al mismo tiempo, envían un mensaje al puerto serie avisando qué tarea está corriendo. El mensaje que se observa es el siguiente:

![](C:\Users\camis\Desktop\Camila\Fiuba\SistemasEmbebidos\TP3\Imagenes\ex1_ps1.PNG)

Los mensajes no se ven correctamente porque ambas tareas poseen igual prioridad y ninguna de ellas se bloquea para que la otra ocupe el tiempo de CPU que necesita. Por esta razón se usa el método de **Time Slicing**, que permite darle una porción del tiempo de la CPU a cada tarea de igual prioridad. Cada tarea recibe un número fijo de ticks. Cuando esa cantidad de ticks es alcanzada, el kernel o scheduler le asigna la misma cantidad de ticks a la siguiente tarea de igual prioridad. De ahí el nombre **Time Slicing** (división de tiempo), ya que se parte el tiempo de la CPU entre todas las tareas de igual prioridad.

El tiempo que se le asigna a cada tarea se configura con la siguiente macro (**configTICK_RATE_HZ**):

![](C:\Users\camis\Desktop\Camila\Fiuba\SistemasEmbebidos\TP3\Imagenes\ex1_ps1_2.PNG)

Previamente, estaba seteada a 1000, de forma que cuando se setee el tiempo, se divida por 1000 y quede siendo de 1 ms. Para aumentar el tiempo, por ejemplo a 100 ms, se puede cambiar el valor de **configTICK_RATE_HZ** a 10 y así, la salida del puerto serie se verá correctamente, como se muestra a continuación:

![](C:\Users\camis\Desktop\Camila\Fiuba\SistemasEmbebidos\TP3\Imagenes\ex1_ps100.PNG)

El diagrama temporal de la distribución del tiempo de la CPU es el siguiente:

![](C:\Users\camis\Desktop\Camila\Fiuba\SistemasEmbebidos\TP3\Imagenes\ex1_2.PNG)

### Ejemplo 3

En el ejemplo 3, ocurre la misma situación pero las tareas se configuran de la siguiente manera:

![](C:\Users\camis\Desktop\Camila\Fiuba\SistemasEmbebidos\TP3\Imagenes\ex3_3.PNG)

Se puede ver que la tarea 2 tiene una prioridad mayor que la 1 y por esta razón, el Kernel le entrega todo el tiempo de la CPU a la tarea de mayor prioridad, en este caso, la 2. Por esto, la salida del puerto serie es la siguiente:

![](C:\Users\camis\Desktop\Camila\Fiuba\SistemasEmbebidos\TP3\Imagenes\ex3_1.PNG)

El diagrama de uso del tiempo de la CPU es el siguiente:

![](C:\Users\camis\Desktop\Camila\Fiuba\SistemasEmbebidos\TP3\Imagenes\ex3_2.PNG)

## Aplicación 1

Todas las aplicaciones se encuentran en  [freertos_app.c](./freertos_examples_10_to_16/example/src).

En la siguiente aplicación se tiene el siguiente diagrama temporal:

![](app1.PNG) 

Se crean tres tareas, de las cuales la Tarea 1 es la periodida. A esta se le asigna a la mayor prioridad, mientras que las otras dos estan al mismo nivel. La Tarea 1 genera un interrupcion cada 500mS. En este interrupcion se da un semaforo binario, el cual desbloquea la Tarea 2. Esta agrega un valor a la cola que desbloquea la Tarea 3, la cual imprime por consola el valor.

El reusltado por consola es el siguiente:

![](app1a.PNG) 

## Aplicación 2

En la siguiente aplicación se tiene el siguiente diagrama temporal:

![](app1.PNG)

La aplicación es similar a la anterior, con la diferencia fundamental que la Tarea 2 ahora se sincroniza con la ISR mediante una cola y la Tarea 2 se sincroniza con la Tarea 3 mediante un semaforo binario.

El reusltado por consola es el siguiente:

![](app2a.PNG) 

## Aplicación 3

En la siguiente aplicación se tiene el siguiente diagrama temporal:

![](app3.PNG)

Se crean tres tares con la misma prioridad que quieren utilizar un recurso en comun (LED + UART). Se les asigna la misma prioridad para que las 3 tengan la misma oportunidad de acceder el recurso. Cada tarea desea hacer encender durante 500 mS y apagar durante 500mS al LED e imprimir por consola su estado. Las tres tareas se crean desde el mismo prototipo, pasandoles distintos paràmetros. 

Se protege el recurso utilizando un Mutex que se toma al comenzar la tarea si se puede, se prende el LED, se tiene una demora (vTaskDelay) de 500ms, se apaga el LED, se tiene una demora igual y se suelta el Mutex.

Las tres tareas ciclan el uso del LED como se observa en la consola:

![](app3a.PNG)
