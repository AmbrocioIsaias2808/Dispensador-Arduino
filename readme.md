 ## Materiales hasta el momento:

* Arduino Uno
* Modulo de reloj DS1302 con libreria <virtuabotixRTC.h>
* Sensor infrarrojo y control remoto
* Servomotor MG995 (Necesita alimentación externa)
* Pantalla LCD  con modulo I2C
* Modulo de marga y sensor HX711. **Notas:**
	* **Librería extraída de:** https://github.com/bogde/HX711 
	* **Leer ejemplo de:** https://naylampmechatronics.com/blog/25_tutorial-trasmisor-de-celda-de-carga-hx711-ba.html y checar las plantillas de ejemplo que puedes encontrar en los archivos de la librería, esto para adaptar código que muestra naylampmechatronics dado que esta desactualizada la información de la pagina, basarse en el ejemplo HX711_basic_example de la librería, solo es modificar unas cuantas líneas para hacerlo funcionar con el código de naylampmechatronics)
  * Información adicional:
  	* https://www.youtube.com/watch?v=6LyJMymRadQ
  	* https://www.youtube.com/watch?v=MpITkBA50k8
* Buzzer

---

**Descripción:** al momento ya es posible....

* Dar comandos mediante control infrarrojo
* Es posible programar hasta 3 alarmas para alimentar a nuestra mascota (Presionando el boton * para configurar)
* Es posible modificar la hora de nuestro reloj (Presionando el boton # para configurar)
* Es posible establecer el tiempo de dispensado. Se tiene pensado que este sea la cantidad de alimento a dispensar en gramos por eso las leyendas del código. (Presionando el boton 0 para configurar)
* Es posible usar el display LCD para interactuar con el sistema.
* Al momento de llegarse el tiempo de alimentar a nuestra mascota, nuestro proyecto avisa haciendo unos pitidos por medio de nuestro buzzer.
* La cantidad de alimento ahora es coordinada por medio de el modulo de carga, de manera que este verifica la cantidad de comida que esta siendo dispensada, cerrando la compuerta una vez que se a alcanzado la cantidad en gramos de comida configurada.

