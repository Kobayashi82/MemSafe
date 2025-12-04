
<div align="center">

![System & Kernel](https://img.shields.io/badge/System-brown?style=for-the-badge)
![Memory & FD Management](https://img.shields.io/badge/Memory%20&%20FD-Management-blue?style=for-the-badge)
![Norminette-OK](https://img.shields.io/badge/Norminette-OK-green?style=for-the-badge)
![C Language](https://img.shields.io/badge/Language-C-red?style=for-the-badge)

*Wrapper functions para gestión segura de memoria y recursos del sistema*

</div>

# MemSafe

## 🎯 Descripción

MemSafe es una biblioteca de funciones wrapper que proporciona una capa de seguridad para la gestión automática de memoria y recursos del sistema en aplicaciones C.

Este proyecto intercepta llamadas estándar al sistema operativo (`malloc`, `free`, `open`, `close`, `execve`, etc.) para gestionar automáticamente la liberación de recursos y prevenir memory leaks en caso de errores o terminación del programa.

## ✨ Características

- **Gestión automática de memoria**: Rastreo y liberación automática de toda la memoria asignada.
- **Gestión de descriptores de archivo**: Control automático de apertura y cierre de file descriptors.
- **Seguridad en `execve`**: Duplicación segura de argumentos y entorno antes de ejecutar nuevos procesos.
- **Hash table eficiente**: Utiliza una tabla hash para el seguimiento rápido de punteros de memoria.
- **Liberación masiva**: Función especial para liberar todos los recursos con una sola llamada.
- **Sin overhead visual**: Los wrappers son transparentes para el código del usuario.
- **Compatible con POSIX**: Implementación estándar compatible con sistemas Unix/Linux.
- **Norminette compliant**: Código que cumple con los estándares de la Norminette de 42.

## 📦 Componentes

### safe_mem.c
Wrapper para funciones de gestión de memoria:
- `__wrap_malloc()` - Intercepta malloc y registra el puntero asignado en una tabla hash
- `__wrap_free()` - Intercepta free y elimina el puntero de la tabla hash
- Función especial: `free((void *)-42)` - Libera automáticamente toda la memoria registrada

**Características técnicas:**
- Tabla hash de 1031 entradas para distribución óptima
- Función de hash personalizada para minimizar colisiones
- Gestión de colisiones mediante listas enlazadas
- O(1) en promedio para inserción y búsqueda
- No libera memoria no asignada (no produce error)
- No libera memoria ya liberada (no produce error)

### safe_fd.c
Wrapper para funciones de manejo de file descriptors:
- `__wrap_open()` - Intercepta open y registra el descriptor en una tabla
- `__wrap_close()` - Intercepta close y actualiza la tabla de descriptores
- `__wrap_dup()` - Intercepta dup y registra el nuevo descriptor
- `__wrap_dup2()` - Intercepta dup2 y gestiona ambos descriptores
- `__wrap_pipe()` - Intercepta pipe y registra ambos extremos del pipe
- Función especial: `close(-42)` - Cierra automáticamente todos los descriptores abiertos

**Características técnicas:**
- Tabla estática de 1024 entradas (0-1023)
- Soporte para flags de `O_CREAT` con modo variádico
- Gestión de descriptores estándar (stdin, stdout, stderr)

### safe_execve.c
Wrapper para función de ejecución:
- `__wrap_execve()` - Intercepta execve, duplica argumentos/entorno y libera recursos automáticamente

**Características técnicas:**
- Duplicación profunda de arrays de strings para `argv` y `envp`
- Uso de funciones `__real_*` para evitar recursión
- Restauración de stdin, stdout, stderr tras liberar recursos
- Gestión segura de memoria en caso de fallo de execve

### safe_exit.c
Wrapper para función de terminación:
- `__wrap_exit()` - Intercepta exit y libera todos los recursos antes de terminar

**Características técnicas:**
- Handlers para 11 señales críticas (SIGTERM, SIGINT, SIGQUIT, SIGHUP, SIGILL, SIGABRT, SIGFPE, SIGSEGV, SIGBUS, SIGTRAP, SIGSYS)
- Uso de `__attribute__((constructor(101)))` para inicialización temprana de handlers
- Registro con `atexit()` para garantizar limpieza en terminación normal
- Restauración de handlers por defecto con `signal(sig, SIG_DFL)` antes de re-lanzar señal
- Llamada a `raise(sig)` para propagar la señal después de limpieza

## 🔧 Instalación y Uso

### Compilación del proyecto de prueba

```sh
make
```

Esto generará el ejecutable `test` con todos los wrappers aplicados.

### Ejecución del test

```sh
./test
```

El programa de prueba demuestra:
- Múltiples asignaciones de memoria
- Apertura de archivos
- Creación de pipes
- Duplicación de descriptores
- Llamada a execve
- Liberación automática de todos los recursos

### Verificación con Valgrind

El proyecto incluye el script `leaks` que ejecuta Valgrind con las opciones apropiadas para detectar memory leaks y file descriptor leaks:

```sh
./leaks ./test
```

Este script ejecuta:
- `--leak-check=full` - Análisis completo de memory leaks
- `--show-leak-kinds=all` - Muestra todos los tipos de leaks
- `--track-fds=yes` - Rastrea file descriptors abiertos
- `--trace-children=yes` - Sigue procesos hijos

### Integración en tu proyecto

Para usar MemSafe en tu propio proyecto, añade las siguientes flags de enlazado:

```makefile
LDFLAGS = -Wl,--wrap=malloc \
          -Wl,--wrap=free \
          -Wl,--wrap=open \
          -Wl,--wrap=close \
          -Wl,--wrap=dup \
          -Wl,--wrap=dup2 \
          -Wl,--wrap=pipe \
          -Wl,--wrap=execve \
          -Wl,--wrap=exit
```

Y añade los archivos fuente a tu compilación:

```makefile
SRCS = safe_mem.c safe_fd.c safe_execve.c safe_exit.c ...
```

### Uso en el código

El código de tu aplicación no necesita cambios. Simplemente usa las funciones estándar:

```c
#include <stdlib.h>
#include <fcntl.h>

int main(void)
{
    char *ptr;
    int fd;
	
	ptr = malloc(100);                // Se registra automáticamente
	fd = open("file.txt", O_RDONLY);  // Se registra automáticamente
    
    // resto de código...
    
    // Al salir, todo se libera automáticamente independientemente de como se salga o:
    // - con free((void *)-42) liberas toda la memoria
    // - con close(-42) cierras todos los descriptores de archivo
}
```

### Liberación manual de todos los recursos

Si necesitas liberar todos los recursos en un punto específico:

```c
// Liberar toda la memoria registrada
free((void *)-42);

// Cerrar todos los descriptores de archivo
close(-42);
```

## 🎓 Casos de uso

### 1. Shells y línea de comandos
Perfecto para implementaciones de shells donde múltiples comandos se ejecutan y es crítico no dejar recursos huérfanos.

### 2. Servidores y daemons
Aplicaciones de larga duración que necesitan gestión robusta de recursos.

### 3. Aplicaciones con execve
Programas que ejecutan otros procesos y necesitan asegurar la limpieza antes de la ejecución.

### 4. Desarrollo y debugging
Herramienta educativa para entender el ciclo de vida de recursos del sistema.

### 5. Testing
Asegurar que los tests no dejen recursos abiertos entre ejecuciones.

## 🏗️ Arquitectura

```
               ┌───────────────────────────────────────┐
               │             Tu Aplicación             │
               └───────────────────┬───────────────────┘
                        ┌──────────▼──────────┐
                        │   MemSafe Wrapper   │
        ┌───────────────└─┬─────────────────┬─┘───────────────┐
 ┌──────▼──────┐   ┌──────▼──────┐   ┌──────▼──────┐   ┌──────▼──────┐
 │  safe_mem.c │   │  safe_fd.c  │   │safe_execve.c│   │ safe_exit.c │
 └──────┬──────┘   └──────┬──────┘   └──────┬──────┘   └──────┬──────┘
        └──────────────►┌─▼─────────────────▼─┐◄──────────────┘
                        │    System  Calls    │
                        └─────────────────────┘
```

## 🔍 Detalles de implementación

### Tabla Hash (safe_mem.c)

La función hash utilizada es una variante del algoritmo de hash multiplicativo:

```c
addr = ~addr + (addr << 15);   // Complemento y shift
addr ^= (addr >> 12);          // XOR con shift derecho
addr += (addr << 2);           // Suma con shift
addr ^= (addr >> 4);           // Más XOR
addr *= 2057;                  // Multiplicación por primo
addr ^= (addr >> 16);          // XOR final
return (addr % 1031);          // Módulo con primo
```

Esta función hash proporciona excelente distribución y minimiza colisiones.

### Gestión de FDs (safe_fd.c)

Los descriptores se almacenan en un array estático de 1024 posiciones, cubriendo el rango típico de descriptores en sistemas Unix (0-1023).

### Duplicación segura (safe_execve.c)

Antes de llamar a `execve`, se duplican todos los argumentos y variables de entorno usando `__real_malloc`, de forma que los recursos originales puedan liberarse de forma segura incluso si `execve` falla.

### Terminación segura (safe_exit.c)

Gestiona la salida del proceso llamando a exit, tanto en finalización normal como en condiciones de error.
Intercepta señales que terminan el programa y libera todos los recursos registrados antes de salir

## ⚠️ Consideraciones

- **Overhead de memoria**: Cada puntero asignado consume memoria adicional (16 bytes) para el nodo de la lista enlazada.
- **Thread-safety**: Esta implementación NO es thread-safe. Para uso multihilo, se requieren mutex.
- **Límite de FDs**: Soporte para un máximo de 1024 descriptores de archivo.
- **Uso de -42**: El valor mágico `-42` se usa para comandos especiales. Evita usar este valor en tu código.
- **Performance**: El overhead es mínimo para la mayoría de aplicaciones, pero puede ser significativo en programas con millones de asignaciones.

## 🔮 Posibles mejoras

- [ ] Soporte para realloc y calloc
- [ ] Implementación thread-safe con mutex
- [ ] Estadísticas de uso de memoria (peak, total, leaks)

---

## 📄 Licencia

Este proyecto está licenciado bajo la WTFPL – [Do What the Fuck You Want to Public License](http://www.wtfpl.net/about/).

---

<div align="center">

**🧠 Desarrollado por Kobayashi82 🧠**

*"Memory leaks? Not on my watch!"*

</div>
