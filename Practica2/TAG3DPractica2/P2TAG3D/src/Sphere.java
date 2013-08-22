/**
 * Sphere.
 */
public class Sphere extends Surface {
	/**
	 * Center.
	 */
	public Vec3						position;
	/**
	 * Radius.
	 */
	public double					radius = -1;

	/**
	 * Inicializar.
	 * Use by the parser.
	 * METODO CREADO POR NOSOTROS
	 */
	public void initFromParser(){
		if (this.position == null){
			System.err.println("Sphere: Falta el centro <position>");
			System.exit(1);
		}
		if (this.position == null){
			System.err.println("Sphere: Falta el radio <radius>");
			System.exit(1);
		}
		this.vmin = new Vec3(1,1,1).scale(-radius);
		this.vmin.setToAdd(position);
		this.vmax = new Vec3(1,1,1).scale(+radius);
		this.vmax.setToAdd(position);
	}


	/**
	 * Intersect the given ray with this scene storing the result in intersection and
	 * result true if there was an intersection.
	 */
	boolean intersect(Ray ray, Intersection intersection) {
		// Calcular el valor de intersección mas cercano
		double t = tvalue(ray);
		if (t<0){ // No hay interseccion
			intersection = null;
			return false;
		}
		// Hay interseccion. calcular sus parametros 
		Vec3 p = ray.evaluate(t);
		Vec3 n = p.sub(this.position).normalize();
		double dist = p.sub(ray.origin).length();
		intersection.position = p;
		intersection.normal = n;
		intersection.material = this.material;
		intersection.distance = dist;
		intersection.object = this;
		// Texturas
		if (this.hasUVs())
			intersection.uv = this.getUV(p);
		if(this.hasBump())
			intersection.normal = this.applyBump(intersection.normal, intersection.uv);
		return true;
	}


	/**
	 * CREADO POR NOSOTROS
	 * Comprueba el valor de t en el que se produce la interseccion. Devuelve -1 si no hay 
	 * @param ray
	 * @return
	 */
	private double tvalue (Ray ray){
		double t = -1;
		if ( Math.abs(ray.direction.dot(ray.direction) - 1.0) > 0.01 )
			System.err.println("WARNING : La direccion del rayo no llega aqui normalizada "+ray.direction.dot(ray.direction));

		/*
		 * Si C es el centro de la esfera, R su radio, O el origen del rayo y D su direccion
		 * La ecuación de la esfera es del tipo:
		 *  (x-Cx)^2+(y-Cy)^2+(z-Cz)^2 = r^2
		 *  
		 *  Sustituyendo la recta en la esfera para calcular los puntos de corte
		 *  a(t)^2 + b(t) + c = 0
		 *  a = D·D	
		 *  b = 2·(D·(O-C))
		 *  c = C·C + O·O - 2·(C·O) - R^2
		 *  
		 *  El numero de soluciones de d=b^2-4ac es el numero de puntos de corte
		 *  Las soluciones son (-b+-D^0.5)/2a   
		 */
		Vec3 D = ray.direction;
		Vec3 O = ray.origin;
		Vec3 C = this.position;

		double a = D.dot(D); // poner a 1 cuando siempre sepamos que D esta normalizado
		double b = 2.0 * (D.dot(O.sub(C)));
		double c = C.dot(C) + O.dot(O) - 2*(C.dot(O)) - this.radius*this.radius;
		double d = b*b - 4.0*a*c;

		if (d < 0){ // No hay intersección
			return -1; 
		}else{ // Calcular los dos puntos de interseccion y devolver el mas cercano
			double divA2 = 1.0/(2.0*a); 
			double sqrtD = Math.sqrt(d);
			double t1 = (-b + sqrtD)*divA2;
			double t2 = (-b - sqrtD)*divA2;

			if (t1 < ray.minDistance || t1 > ray.maxDistance){ 
				t1 = -1; // No cuenta como intersección
				t = t2;
			}
			if (t2 < ray.minDistance || t2 > ray.maxDistance){
				t2 = -1; // No cuenta como intersección
				t = t1;
			}
			if (t1 >= 0 && t2 >= 0){ // Elegir el menor t
				t = t1 < t2 ? t1 : t2;	
			}

		}return t;
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
		Vec3 vp = p.sub(this.position);
		double rtx = vp.x;
		double rty = vp.z;
		double rtz = vp.y;
		Vec3 uv = new Vec3();

		double phi = Math.acos(rtz/radius);
		double theta;
		double S = Math.sqrt(rtx * rtx + rty * rty);
		if (rtx >= 0)
			theta = Math.asin(rty / S);
		else
			theta = Math.PI - Math.asin(rty / S);

		if (theta < 0) theta = 2.0 * Math.PI + theta;

		uv.x = theta/(2*Math.PI);
		uv.y = -phi/(Math.PI);;

		return uv;

	}

	/**
	 * METODO CREADO POR NOSOTROS
	 */
	public String toString(){
		return "Sphere";
	}
}
