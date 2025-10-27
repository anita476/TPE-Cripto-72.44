# TPE-Cripto-72.44
Trabajo Especial de Implementación para Criptografía y Seguridad (72.44) @ITBA

## Integrates
- **[Ana Negre](https://github.com/anita476)** - [63074]
- **[Matías Leporini](https://github.com/LosProgramasdeLepo)** - [62872]
- **[Camila Lee](https://github.com/clee0412)** - [63382]

## Tabla de Contenidos
- [Descripción](#descripción)
- [Requisitos del Sistema](#requisitos-del-sistema)
- [Instalación y Compilación](#instalación-y-compilación)
- [Uso del Programa](#uso-del-programa)
- [Referencias](#referencias)

## Descripción
Implementación de algoritmos de esteganografía LSB (Least Significant Bit) para ocultar información en imágenes BMP de 24 bits sin compresión, según las especificaciones del Trabajo Práctico de la materia Criptografía y Seguridad (72.44).

### Algoritmos Implementados
| Algoritmo | Descripción | Capacidad |
|-----------|-------------|-----------|
| **LSB1** | 1 bit por byte de imagen | N/8 bytes |
| **LSB4** | 4 bits por byte de imagen | N/2 bytes |
| **LSBI** | LSB mejorado (Majeed & Sulaiman) | N/8 bytes |

### Funcionalidades

**Cifrado opcional:**
- Algoritmos: AES-128, AES-192, AES-256, 3DES
- Modos: ECB, CBC, CFB (8 bits), OFB (128 bits)
- Derivación de clave: PBKDF2-HMAC-SHA256 con 10,000 iteraciones
- Salt fijo: `0x0000000000000000`
- Padding: PKCS#5

**Características:**
- Preservación automática de la extensión del archivo oculto
- Formato de almacenamiento: tamaño (4 bytes big-endian) + datos + extensión + '\0'
- Soporte para cualquier tipo de archivo

## Requisitos del Sistema

### Plataforma
- Ubuntu 22.04 LTS (nativo o WSL2)
- Sistema operativo compatible con OpenSSL 3.0.x

### Dependencias
```bash
# Actualizar repositorios
sudo apt update

# Herramientas de compilación
sudo apt install -y build-essential

# Librerías de OpenSSL (requeridas)
sudo apt install -y libssl-dev openssl
```


## Instalación y Compilación

### Método 1: Usando Docker (Recomendado)
El contenedor Docker proporciona un ambiente Ubuntu 22.04 con todas las dependencias preinstaladas (OpenSSL 3.0, build-essential, etc.).

#### Primera vez (instalación completa)
```bash
# Construir la imagen y entrar al contenedor automáticamente
cd script/
./install.sh

# Una vez dentro del contenedor, compilar el proyecto
cd /home
make clean
make

# Verificar que el ejecutable se creó correctamente
ls -lh stego
./stego --help
```

#### Usos posteriores
```bash
# Si ya instalaste previamente, solo ejecuta el contenedor
cd script/
./run-container.sh

# Dentro del contenedor, compilar si hay cambios en el código
cd /home
make

# Ejecutar el programa
./stego [opciones]
```

### Método 2: Compilación Local
```bash
# Paso 1: Clonar el repositorio
git clone https://github.com/anita476/TPE-Cripto-72.44.git
cd TPE-Cripto-72.44

# Paso 2: Compilar
make clean
make

# Paso 3: Verificar el ejecutable
./stego --help
```

## Uso del Programa
### Sintaxis General
**Ocultar información (embed):**
```bash
./stego -embed -in <file> -p <bitmapfile> -out <bitmapfile> -steg <LSB1|LSB4|LSBI> [-a <aes128|aes192|aes256|3des>] [-m <ecb|cfb|ofb|cbc>] [-pass <password>]
```


**Extraer información (extract):**
```bash
./stego -extract -p <bitmapfile> -out <file> -steg <LSB1|LSB4|LSBI> [-a <aes128|aes192|aes256|3des>] [-m <ecb|cfb|ofb|cbc>] [-pass <password>]
```


### Parámetros

| Parámetro | Descripción | Requerido | Valores |
|-----------|-------------|-----------|---------|
| `-embed` | Modo ocultar información | Sí* | - |
| `-extract` | Modo extraer información | Sí* | - |
| `-in <file>` | Archivo a ocultar | Sí (embed) | Ruta del archivo |
| `-p <bmp>` | Imagen BMP portadora | Sí | Archivo .bmp |
| `-out <file>` | Archivo de salida | Sí | Ruta de salida |
| `-steg <alg>` | Algoritmo esteganográfico | Sí | `LSB1`, `LSB4`, `LSBI` |
| `-a <alg>` | Algoritmo de cifrado | No | `aes128`, `aes192`, `aes256`, `3des` |
| `-m <mode>` | Modo de cifrado | No | `ecb`, `cbc`, `cfb`, `ofb` |
| `-pass <pwd>` | Contraseña | Condicional** | String |

**Notas:**
- \* Debe especificarse exactamente uno: `-embed` o `-extract`
- \*\* Obligatorio si se usa `-a` o `-m`
- Si se usa `-pass` sin especificar `-a` o `-m`, el programa usa `aes128-cbc` por defecto

### Ejemplos
```bash
# Ocultar sin cifrado
./stego -embed -in mensaje.txt -p imagen.bmp -out salida.bmp -steg LSB1

# Extraer sin cifrado
./stego -extract -p salida.bmp -out mensaje_extraido -steg LSB1

# Ocultar con cifrado
./stego -embed -in mensaje.txt -p imagen.bmp -out salida.bmp -steg LSBI -a aes256 -m cbc -pass "secreto"

# Extraer con cifrado
./stego -extract -p salida.bmp -out mensaje_extraido -steg LSBI -a aes256 -m cbc -pass "secreto"
```

## Referencias
### Paper Principal
Majeed, M. A., & Sulaiman, R. (2013). "An Improved LSB Image Steganography Technique using bit-inverse in 24 bit colour image." Journal of Theoretical and Applied Information Technology, 80(2) (https://www.jatit.org/volumes/Vol80No2/16Vol80No2.pdf)

### Especificaciones Técnicas
* [Formato BMP V3](https://www.di.unito.it/~marcog/SM/BMPformat-Wiki.pdf#:~:text=headers%20in%20the%20popular%20Windows%20V3%20BMP,file%20header%20plus%2040%2Dbyte%20DIB%20V3%20header)
* [Test Images Repository](https://www.hlevkin.com/hlevkin/06testimages.htm)
