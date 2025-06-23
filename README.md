# RDC2

Proyecto de Redes de las Computadora II. Consiste en hacer un socket FTP basico con soporte para los comandos `USER`, `PASS`, `QUIT`, `SYST`, `TYPE`, `PORT`, `RETR`, `STOR`, `NOOP`.

## Codigo original

Este proyecto se basa en el esqueleto de [miniftp](https://github.com/pubdigital/miniftp) de pubdigital. Se hicieron algunas leves modificaciones a lo largo del codigo y se agregaron algunas funciones dentro de handlers.c

## Explicacion de comandos

- **USER _user_**  
  Comando para enviar el nombre de usuario al servidor FTP. Es el primer paso del proceso de autenticación.

- **PASS _password_**  
  Comando para enviar la contraseña correspondiente al usuario especificado con `USER`. Completa el proceso de autenticación.

- **QUIT**  
  Termina la sesión FTP y cierra la conexión con el servidor.

- **SYST**  
  Solicita información sobre el sistema operativo del servidor FTP. Devuelve el tipo de sistema (Linux, MacOS, Windows). 

- **TYPE _A|I_**  
  Establece el tipo de transferencia de datos.  
  `A` (ASCII) para archivos de texto.  
  `I` (Image/binary) para archivos binarios.

- **PORT _h1,h2,h3,h4,p1,p2_**  
  Especifica la dirección IP y el puerto al que el servidor debe conectarse para la transferencia de datos en modo activo.

- **RETR _filename_**  
  Solicita la descarga de un archivo del servidor FTP al cliente.

- **STOR _filename_**  
  Solicita la carga de un archivo desde el cliente al servidor FTP.

- **NOOP**  
  No realiza ninguna acción, simplemente verifica que la conexión con el servidor sigue activa (mantiene la sesión abierta).

## Instruccions Copilacion

Each version has its own `Makefile`. To compile the current version:

```bash
cd iterative
make
make clean
```

## Testing

You can test with any FTP client, e.g.:

```bash
ftp localhost 2121
```
Or using netcat:

```bash
nc localhost 2121
```