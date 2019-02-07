# Motor Berechnung

## Model 2 v16 Parameter
    Area	1.351E+05 mm^2
    Density	0.003 g / mm^3
    Mass	379.229 g
    Volume	1.324E+05 mm^3
    Physical Material	(Various)

    Bounding Box
    Length 	 187.827 mm
    Width 	 316.50 mm
    Height 	 88.287 mm
    World X,Y,Z	0 mm, 0 mm, 0 mm
    Center of Mass	-3.83152 mm, 104.733 mm, 8.51491 mm

    Moment of Inertia at Center of Mass   (g mm^2)
    Ixx = 2.440E+06
    Ixy = 3.221E+04
    Ixz = -1.799E+04
    Iyx = 3.221E+04
    Iyy = 4.456E+05
    Iyz = -3.243E+05
    Izx = -1.799E+04
    Izy = -3.243E+05
    Izz = 2.615E+06

    Moment of Inertia at Origin   (g mm^2)
    Ixx = 6.627E+06
    Ixy = 1.844E+05
    Ixz = -5621.705
    Iyx = 1.844E+05
    Iyy = 4.787E+05
    Iyz = -6.625E+05
    Izx = -5621.705
    Izy = -6.625E+05
    Izz = 6.780E+06


## Haltemoment
### Distanz der Drehachse zum Schwerpunkt
    l = 104.581 mm = 0.104581m
### Gewichtskraft
    Gewicht von Modell Parameter 379.229g
    m = 0.38kg
    F = 9.81 N/kg * 0.38kg = 3.7278N
` `

    M_g = l * F = 0.104581m * 4.7088N = 0.3898570518Nm

## Beschleunigungsmoment
### Trägheitsmoment
    Izz aus Modell Parameter
    J = 0,00678 kgm^2

### Winkelbeschleunigung

    etwa 30° in 0.6s

    α = rad/s^2 = 30*pi/180 / (0.6)
    α = 2 * s / t^2 = 2 * 30 * pi/180 / 0.6^2 = 2.90888208666 rad / s^2
` `

    M_b = α * J = 2.9rad/s^2 * 0.00678kgm^2 = 0.01972222054Nm
## Motordrehmoment Insgesamt
    M_tot = M_g + M_b = 0.3898570518Nm + 0.01972222054Nm = 0.40957927234Nm

### Referenzen
[Trägheitsmoment](http://www.maschinenbau-wissen.de/skript3/mechanik/kinetik/293-traegheitsmoment)  
[Maxon Formelsammlung](https://www.maxonmotor.de/medias/sys_master/root/8819062800414/maxon-Formelsammlung-d.pdf)  

### Motoren
- futaba s3003 
- robbe fs100 