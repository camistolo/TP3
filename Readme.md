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

![](https://github.com/camistolo/TP3/blob/master/Imagenes/ex1_1.PNG?raw=true)

Se setea un puntero que apunta a la función que implementa la tarea, se le asigna un nombre a la tarea, para facilitar el debug y se configuran otros parametros, como por ejemplo, la prioridad, que en este caso, y en el de la tarea 2 de igual forma, es 1.


La tarea 1 apaga el LED3 y la tarea 2 lo prende. Al mismo tiempo, envían un mensaje al puerto serie avisando qué tarea está corriendo. El mensaje que se observa es el siguiente:

![](https://github.com/camistolo/TP3/blob/master/Imagenes/ex1_ps1.PNG?raw=true)

Los mensajes no se ven correctamente porque ambas tareas poseen igual prioridad y ninguna de ellas se bloquea para que la otra ocupe el tiempo de CPU que necesita. Por esta razón se usa el método de **Time Slicing**, que permite darle una porción del tiempo de la CPU a cada tarea de igual prioridad. Cada tarea recibe un número fijo de ticks. Cuando esa cantidad de ticks es alcanzada, el kernel o scheduler le asigna la misma cantidad de ticks a la siguiente tarea de igual prioridad. De ahí el nombre **Time Slicing** (división de tiempo), ya que se parte el tiempo de la CPU entre todas las tareas de igual prioridad.

El tiempo que se le asigna a cada tarea se configura con la siguiente macro (**configTICK_RATE_HZ**):

![](https://github.com/camistolo/TP3/blob/master/Imagenes/ex1_ps1_2.PNG?raw=true)

Previamente, estaba seteada a 1000, de forma que cuando se setee el tiempo, se divida por 1000 y quede siendo de 1 ms. Para aumentar el tiempo, por ejemplo a 100 ms, se puede cambiar el valor de **configTICK_RATE_HZ** a 10 y así, la salida del puerto serie se verá correctamente, como se muestra a continuación:

![](https://github.com/camistolo/TP3/blob/master/Imagenes/ex1_ps100.PNG?raw=true)

El diagrama temporal de la distribución del tiempo de la CPU es el siguiente:

![](https://github.com/camistolo/TP3/blob/master/Imagenes/ex1_2.PNG?raw=true)

### Ejemplo 3

En el ejemplo 3, ocurre la misma situación pero las tareas se configuran de la siguiente manera:

![](https://github.com/camistolo/TP3/blob/master/Imagenes/ex3_3.PNG?raw=true)

Se puede ver que la tarea 2 tiene una prioridad mayor que la 1 y por esta razón, el Kernel le entrega todo el tiempo de la CPU a la tarea de mayor prioridad, en este caso, la 2. Por esto, la salida del puerto serie es la siguiente:

![](https://github.com/camistolo/TP3/blob/master/Imagenes/ex3_1.PNG?raw=true)

El diagrama de uso del tiempo de la CPU es el siguiente:

![](https://github.com/camistolo/TP3/blob/master/Imagenes/ex3_2.PNG?raw=true)

### Ejemplo 6

En este ejemplo, se combinan tres tareas, una de ellas siendo periódica (tarea 3). Se configuran las tareas de la siguiente manera:

![](https://github.com/camistolo/TP3/blob/master/Imagenes/ex6_5.PNG?raw=true)

Las tareas continuas se configuran mediante la función **vContinuousProcessingTask**, que simplemente cambia el LED3 de estado y posee un delay con un for que permite que se observe el mensaje por el puerto serie, ya que si no, el tiempo no sería suficiente. Esta función se ve a continuación:

![](https://github.com/camistolo/TP3/blob/master/Imagenes/ex6_2.PNG?raw=true)

La función periódica se configura utilizando la función **vPeriodicTask**, que utiliza la función **vTaskDelayUntil**, entregándole el valor del tiempo en que se generó la misma señal. Esto se observa a continuación:

![](https://github.com/camistolo/TP3/blob/master/Imagenes/ex6_4.PNG?raw=true)

La función **vTaskDelayUntil** toma el primer parámetro que le entregan y le suma el segundo parámetro, para calcular el tiempo de delay correspondiente.

![](https://github.com/camistolo/TP3/blob/master/Imagenes/ex6_3.PNG?raw=true)

De esta forma, la tarea 3 se ejecuta cada 10 ms, mientras que las tareas 1 y 2 se ejecutan controladas por el kernel, siendo la 2 de mayor prioridad que la 1. El diagrama de tiempo de la CPU se observa a continuación:

![](https://github.com/camistolo/TP3/blob/master/Imagenes/ex6_6.PNG?raw=true)

## Ejemplo 10 

La tarea Receiver es la que se ejecuta en primer lugar ya que es la que tiene mayor prioridad. Esta intentara leer la cola, y si esta esta vacia, la tarea pasará al estado BLOKED hasta que haya un dato en la cola, dejando ejecutar tareas de menor priordad. Luego el CPU se pasa a la tarea Sender2, donde carga un valor en la cola a traves de la función xQueueSendToBack(), por lo tanto la tarea Receiver pasa al estado READY, y como esta ultima tiene mayor prioridad se ejecuta hasta que vacia la cola y pasa al estado BLOCKED. Luego delega el CPU a la tarea Sender2 que cede a este mismo a la tarea Sender1 por medio de la función taskYIELD(). Una vez que la tarea Sender1 escribe en la cola se vuelve a repetir el proceso. 

![](https://github.com/camistolo/TP3/blob/master/Imagenes/ej10.PNG)




## Aplicación 1

Todas las aplicaciones se encuentran en  [freertos_app.c](./freertos_examples_10_to_16/example/src).

En la siguiente aplicación se tiene el siguiente diagrama temporal:

![](app1.PNG) 

Se crean tres tareas, de las cuales la Tarea 1 es la periodida. A esta se le asigna a la mayor prioridad, mientras que las otras dos estan al mismo nivel. La Tarea 1 genera un interrupcion cada 500mS. En este interrupcion se da un semaforo binario, el cual desbloquea la Tarea 2. Esta agrega un valor a la cola que desbloquea la Tarea 3, la cual imprime por consola el valor.

El resultado por consola es el siguiente:

![](https://github.com/camistolo/TP3/blob/master/Imagenes/app1a.PNG?raw=true) 

APP2: Idem app1 intercambiando cola <-> semaforo.

El diagrama temporal de esta aplicacion es el siguiente (del enunciado):

![](https://github.com/camistolo/TP3/blob/master/Imagenes/diagrama_app2.PNG) 

Se muestra las funciones para las tres tareas:

Para la tarea 1, se tiene el mismo que en la app1, una tarea periodica de periodo 500mS:

![](https://github.com/camistolo/TP3/blob/master/Imagenes/TASK1_app2.PNG)

Para la tarea 2, se tiene la misma que en la app1 pero ahora se sincroniza a traves de una cola, no de un semaforo:

![](https://github.com/camistolo/TP3/blob/master/Imagenes/TASK2_app2.PNG)

Por ultimo, para la tarea 3, se tiene la misma que en la app1 pero sincronizandola mediante un semaforo:

![](https://github.com/camistolo/TP3/blob/master/Imagenes/TASK3_app2.PNG)

Se muestra tambien una captura del main. Es igual al de la app1. La diferencia de esta app con la primera esta en las funciones de la tareas en si:

![]https://github.com/camistolo/TP3/blob/master/Imagenes/(main_app2.PNG)

Ejecutado el codigo se obtuvo la siguiente respuesta:

![](https://github.com/camistolo/TP3/blob/master/Imagenes/resultado_app2.PNG)

APP3: 3 tareas que comparten el uso del led, envian una secuencia de unos y ceros. Estas no deben mezclarse

El diagrama temporal sacado del enunciado debe ser el siguiente:

![](https://github.com/camistolo/TP3/blob/master/Imagenes/diagrama_app3.PNG)

En primer lugar, se muestra la funcion encargada de crear las tareas:

![](https://github.com/camistolo/TP3/blob/master/Imagenes/TASK_app3.PNG)

En esta se puede ver que al entrar a la funcion, la tarea toma el mutex al comenzar para bloquear que las otras tareas que quieran usar el mismo recurso no se superpongan, y lo libera una vez que termino lo que tiene que hacer. Luego, se imprime el nombre de la tarea y se prende el led. Se espera un tiempo de 500mS, se apaga el led y se esperan otros 500mS. Finalizado esto, entrega el Mutex.

A continuacion se muetra el main:

![](https://github.com/camistolo/TP3/blob/master/Imagenes/main_app3.PNG)

En este se puede ver que al comenzar se crea el mutex que se utilizara como proteccion para las tareas que esten usando los recursos. Luego se crean las tres tareas usando la misma funcion con la misma prioridad asi tienen la misma chance de usar los recursos (LED y UART), pero se les pasa distinto nombre como parametro. Por ultimo se llama al scheduler.

Al correr la aplicacion, se observa el siguiente resultado en consola:

![](https://github.com/camistolo/TP3/blob/master/Imagenes/resultado_app3.PNG)
