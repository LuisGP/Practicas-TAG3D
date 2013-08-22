/**
 * CLASE CREADA POR NOSOTROS
 * Cylinder.
 */
public class Cylinder extends Surface {
	/**
	 * Bottom Center.
	 */
	public Vec3					center1;
	/**
	 * Top Center.
	 */
	public Vec3					center2;
	/**
	 * radius.
	 */
	public double				radius=-1;
	/**
	 * cap1 normal.
	 */
	public Vec3					normal1;
	/**
	 * cap2 normal.
	 */
	public Vec3					normal2;
	/**
	 * height.
	 */
	public double				height;


	/**
	 * Set the normal height and center positions according to parameters.
	 * Use by the parser.
	 */
	public void initFromParser() {
		if (this.center1 == null){
			System.err.println("Cylinder: Falta la posición de la base <center1>");
			System.exit(1);
		}
		if (this.radius < 0){
			System.err.println("Cylinder: Falta el radio <radius>");
			System.exit(1);
		}
		// Cilindro definido por punto-altura
		if (height != 0){
			center2 = center1.add(new Vec3(0,height,0));
			normal1 = (center1.sub(center2)).normalize();
			normal2 = normal1.negate();
			this.vmin = new Vec3(-radius,0,-radius);
			this.vmin.setToAdd(this.center1);
			this.vmax = new Vec3(radius,height,radius);
			this.vmax.setToAdd(this.center1); 
		}else if (center2 != null){
			// Cilindro definido por dos puntos
			normal1 = (center1.sub(center2)).normalize();
			normal2 = normal1.negate();
			this.height = this.center2.sub(this.center1).length();
			// Como es complicado meterlo en una caja, lo aproximamos con dos esferas
			this.vmin = new Vec3(-radius,-radius,-radius);
			Vec3 v01 = this.vmin.add(this.center1);
			Vec3 v02 = this.vmin.add(this.center2);
			this.vmin.x = Math.min(v01.x, v02.x);
			this.vmin.y = Math.min(v01.y, v02.y);
			this.vmin.z = Math.min(v01.z, v02.z);
			this.vmax = new Vec3(radius,height,radius);
			Vec3 v11 = this.vmax.add(this.center1);
			Vec3 v12 = this.vmax.add(this.center2);
			this.vmax.x = Math.max(v11.x, v12.x);
			this.vmax.y = Math.max(v11.y, v12.y);
			this.vmax.z = Math.max(v11.z, v12.z);
		}else{
			System.err.println("Cylinder: Falta la altura o la posición de la otra tapa <height> ó <center2>");
			System.exit(1);
		}
	}

	/**
	 * Calcular interseccion recta-plano con las "tapas"
	 * N normal
	 * V vertice
	 * P origen del rayo
	 * D direccion del rayo
	 * ti = (N·(V-P))/(N·D)
	 * 
	 * @param ray
	 * @param cap1 true para la tapa 1, false para la 2
	 * @return
	 */

	private double tvalueCap(Ray ray, boolean cap1){
		Vec3 N = null;
		Vec3 V = null;

		if (cap1){
			N = this.normal1;
			V = this.center1;
		}else{
			N = this.normal2;
			V = this.center2;			
		}
		Vec3 P = ray.origin;
		Vec3 D = ray.direction;
		double NdotD = N.dot(D);
		if (NdotD == 0) // Rayo y plano paralelos
			return -1;

		double t = (N.dot(V.sub(P)))/(NdotD);

		// Interseccion por detras del origen se descarta
		if (t < ray.minDistance || t > ray.maxDistance) 
			return -1;

		return t;
	}


	private double checkLateral(Ray ray, double t){
		// Descartamos las intersecciones fuera de límite
		if (t < ray.minDistance || t > ray.maxDistance){
			t = -1;
		}

		Vec3 E = this.normal2;
		/*
		 * Y además debe cumplir, para no sobrepasar las tapas
		 * E·(Pt-C1) > 0
		 * E·(Pt-C2) < 0 
		 */ 
		if (t > -1){
			Vec3 Pt3 = ray.evaluate(t);
			double EPT_C1 = E.dot(Pt3.sub(this.center1));
			double EPT_C2 = E.dot(Pt3.sub(this.center2));
			double margen = ray.minDistance;
			if ( EPT_C1 < margen || EPT_C2 > margen )
				t = -1;
		}
		return t;
	}



	/**
	 * Intersect the given ray with this scene storing the result in intersection and
	 * result true if there was an intersection.
	 */
	public boolean intersect(Ray ray, Intersection intersection) {
		/*
		 * Calcular interseccion recta-plano con las "tapas"
		 */
		double t1 = this.tvalueCap(ray, true);
		double t2 = this.tvalueCap(ray, false);
		double t31 = -1;
		double t32 = -1;
		int nt = -1;

		Vec3 Pt1 = ray.evaluate(t1);
		Vec3 Pt2 = ray.evaluate(t2);


		/*
		 * Comprobar si pertenecen a las tapas realmente 
		 */
		// Vamos a dar un poco de margen...
		double margen = ray.minDistance;
		double dist1 = (Pt1.sub(this.center1)).length();
		double dist2 = (Pt2.sub(this.center2)).length();
		t1 = dist1 <= this.radius+margen ? t1 : -1;
		t2 = dist2 <= this.radius+margen ? t2 : -1;

		/*
		 * Los puntos que cumplen la ecuación del cilindro en el lateral son las
		 * soluciones de una ecuación cuadrática como en la esfera
		 * Siendo E la dirección del eje, C el centro de la tapa de abajo,
		 * D la dirección del rayo O el origen del rayo y R el radio,
		 * Los coeficientes a,b y c son, respectivamente:
		 * a = (D-(D·E)E)^2
		 * b = 2( (D-((D·E)E)) · ((O-C)-((O-C)·E)E) )
		 * c = (((O-C) - ((O-C)-((O-C)·E)E))^2 - R^2
		 * 
		 */
		Vec3 E = this.normal2;
		Vec3 C = this.center1;
		Vec3 O = ray.origin;
		Vec3 D = ray.direction;
		double R = this.radius;
		Vec3 O_C = O.sub(C);
		Vec3 D_DEE = D.sub(E.scale((D.dot(E))));
		Vec3 O_C_O_CEE = O_C.sub(E.scale(O_C.dot(E))); 

		double a = D_DEE.dot(D_DEE);
		double b = 2*(D_DEE.dot(O_C_O_CEE));
		double c = O_C_O_CEE.dot(O_C_O_CEE) - R*R;
		double discr = b*b - 4.0*a*c;

		if (discr > 0){ // hay intersección
			// Calcular los dos puntos de interseccion
			double divA2 = 1.0/(2.0*a); 
			double sqrtD = Math.sqrt(discr);
			t31 = (-b + sqrtD)*divA2;
			t32 = (-b - sqrtD)*divA2;

			// Descartamos las intersecciones fuera de límite
			t31 = this.checkLateral(ray, t31);
			t32 = this.checkLateral(ray, t32);
		}

		// Elegir el mejor t de los 4 posibles
		double t = ray.maxDistance+1;
		if ( t1>-1 && t1<t )	{	nt = 1;	t = t1;		}
		if ( t2>-1 && t2<t )	{	nt = 2;	t = t2;		}
		if ( t31>-1 && t31<t )	{	nt = 31;t = t31;	}
		if ( t32>-1 && t32<t )	{	nt = 32;t = t32;	} 

		if (nt == -1) // No hay ninguna intersección
			return false;

		/*
		 * Cada intersección  se trata de forma distinta
		 */
		Vec3 normal = null;
		Vec3 Pt = ray.evaluate(t);
		Vec3 auxn1 = null;
		double auxn2 = 0.0;
		switch (nt){	
		case 1 : // Tapa de abajo
			normal = this.normal1;
			break;

		case 2 : // Tapa de arriba
			normal = this.normal2;
			break;

		case 31 : // Lateral
		case 32 :
			auxn1 = Pt.sub(this.center1); 
			auxn2 = auxn1.dot(this.normal2);// proyección
			Vec3 ptoEje = this.center1.add(this.normal2.scale(auxn2));
			normal =  Pt.sub(ptoEje); // Si ha ido bien, el módulo es R
			normal = normal.normalize();
			break;
		}

		// Hay intersección. calcular sus parametros 
		double dist = Pt.sub(ray.origin).length();
		intersection.position = Pt;
		intersection.normal = normal;
		intersection.material = this.material;
		intersection.distance = dist;
		intersection.object = this;

		// texturas
		if (this.hasUVs())
			intersection.uv = this.getUV(Pt);
		if(this.hasBump())
			intersection.normal = this.applyBump(intersection.normal, intersection.uv);
		return true;
	}

	
	/**
	 * Intersección de refracción.
	 * No es necesario ningún caso especial
	 */
	public boolean intersectRefr(Ray ray, Surface s, Intersection intersection) {
		return intersect(ray, intersection);
	}
	
	/**
	 * Devuelve las coordenadas de textura del punto en concreto.
	 * METODO CREADO POR NOSOTROS
	 * @param p
	 * @return
	 */
	public Vec3 getUV(Vec3 p){
		Vec3 p1 = p;
		// Los casos en los que la X es cero dan error. 
		// Desplazarlos ligeramente los soluciona
		if (p.x==0){
			p1 = p.add(this.normal2.scale(Ray.EPSILON));
			if (p1.x == 0)
				p1.setToAdd(new Vec3(Ray.EPSILON,0,0));
		}
		// altura respecto de la base
		double EPT_C1 = this.normal2.dot(p1.sub(this.center1));
		Vec3 uv = new Vec3();
		Vec3 pnb2 = null;
		// Eje en vertical
		if (Math.abs(normal2.y) > 1-Ray.EPSILON ){ 
			pnb2 = p1.sub(center1);
		}else{ // Eje no vertical. Cambiar la orientación
			Vec3 pn = p1.add(normal1.scale(EPT_C1)).sub(center1);
			pnb2 = new Vec3();
			Vec3 base2[] = new Vec3[3];
			base2[0] = new Vec3(0,1,0);
			base2[1] = this.normal2;
			base2[2] = base2[0].cross(base2[1]).normalize();
			pnb2.x = pn.dot(base2[0]);
			pnb2.y = pn.dot(base2[1]);
			pnb2.z = pn.dot(base2[2]);
		}
		// El ángulo es U
		if (pnb2.z > 0)
			uv.x = Math.atan(pnb2.x/pnb2.z)/(2*Math.PI);
		else
			uv.x = 0.5+Math.atan(pnb2.x/pnb2.z)/(2*Math.PI);
		uv.x *= -1.0;

		// La altura es V
		uv.y = EPT_C1/this.height;
		if (uv.y > 1) uv.y = 1;
		if (uv.y < 0) uv.y = 0;
		return (uv);
	}

	/**
	 * Para las estadísticas.
	 */
	public String toString(){
		return "Cylinder";
	}
}
