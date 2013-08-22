/**
 * Triangle.
 */
public class Triangle extends Surface {
	/**
	 * Vertex 0.
	 */
	public Vec3					v0;
	/**
	 * Vertex 1.
	 */
	public Vec3					v1;
	/**
	 * Vertex 2.
	 */
	public Vec3					v2;
	/**
	 * Normal.
	 */
	public Vec3					normal;
	/**
	 * Coordenadas de textura. ATRIBUTO AÑADIDO POR NOSOTROS
	 */
	public Vec3					v0t;
	/**
	 * Coordenadas de textura. ATRIBUTO AÑADIDO POR NOSOTROS
	 */
	public Vec3					v1t;
	/**
	 * Coordenadas de textura. ATRIBUTO AÑADIDO POR NOSOTROS
	 */
	public Vec3					v2t;

	/**
	 * Calcula el doble del área del triangulo.
	 * METODO CREADO POR NOSOTROS
	 * @return
	 */
	protected double area2(){
		Vec3 l1 = this.v1.sub(this.v0);
		Vec3 l2 = this.v2.sub(this.v0);
		return l1.cross(l2).length();
	}

	/**
	 * Calcula las coordenadas baricéntricas (no las normaliza).
	 * METODO CREADO POR NOSOTROS
	 * @return
	 */
	protected Vec3 barCoords(Vec3 p){
		Triangle t2 = new Triangle();
		t2.v0 = p;
		t2.v1 = this.v1;
		t2.v2 = this.v2;
		double area2 = t2.area2();
		Triangle t3 = new Triangle();
		t3.v0 = this.v0;
		t3.v1 = p;
		t3.v2 = this.v2;
		double area3 = t3.area2();
		Triangle t4 = new Triangle();
		t4.v0 = this.v0;
		t4.v1 = this.v1;
		t4.v2 = p;
		double area4 = t4.area2();

		return new Vec3(area2,area3,area4);
	}	

	/**
	 * Inicializar.
	 * Use by the parser.
	 */
	public void initFromParser() {
		if (this.material == null){
			System.err.println("Triangle: Falta definir el material <material>");
			System.exit(1);
		}
		if (this.v0 == null || this.v1 == null || this.v2 == null){
			System.err.println("Triangle: Falta algún vértice <v0><v1><v2>");
			System.exit(1);
		}
		if (this.material.hasTexture() && (this.v0t == null || this.v1t == null || this.v2t == null)){
			System.err.println("Triangle: Faltan coordenadas de textura <v0t><v1t><v2t>");
			System.exit(1);
		}
		if (this.hasBump() && (this.v0t == null || this.v1t == null || this.v2t == null)){
			System.err.println("Triangle: Faltan coordenadas de textura <v0t><v1t><v2t>");
			System.exit(1);
		}
		normal = (v1.sub(v0)).cross(v2.sub(v0)).normalize();
	}

	/**
	 * Intersect the given ray with this scene storing the result in intersection and
	 * result true if there was an intersection.
	 */
	public boolean intersect(Ray ray, Intersection intersection) {
		/*
		 * Calcular interseccion recta-plano
		 * N normal
		 * V vertice
		 * P origen del rayo
		 * D direccion del rayo
		 * ti = (N·(V-P))/(N·D)
		 */ 
		Vec3 N = this.normal;
		Vec3 V = this.v0;
		Vec3 P = ray.origin;
		Vec3 D = ray.direction;
		double NdotD = N.dot(D);
		if (NdotD == 0) // Rayo y plano paralelos
			return false;

		double ti = (N.dot(V.sub(P)))/(NdotD);
		if ( ti < ray.minDistance || ti > ray.maxDistance) // Interseccion por detras del origen se descarta
			return false;

		Vec3 Pti = ray.evaluate(ti);
		/*
		 * Calcular el area de los triangulos posibles con v0...v3 y el nuevo punto r(ti)
		 * Si es igual la suma entonces esta dentro 
		 */

		// coord. baricentricas en 3D
		double area = this.area2();
		Vec3 bars = this.barCoords(Pti);
		double areas = bars.x+bars.y+bars.z;

		// Vamos a dar un poco de margen...
		double margen = ray.minDistance;

		if (areas > area + margen) // La interseccion es fuera del triangulo
			return false;

		// Hay interseccion. calcular sus parametros 
		double dist = Pti.sub(ray.origin).length();
		intersection.position = Pti;
		intersection.normal = this.normal;
		intersection.material = this.material;
		intersection.distance = dist;
		intersection.object = this;
		// Texturas
		if (this.hasUVs())
			intersection.uv = this.getUV(Pti);
		if(this.hasBump())
			intersection.normal = this.applyBump(intersection.normal, intersection.uv);

		return true;
	}

	/**
	 * Intereseccion por refracción
	 */
	public boolean intersectRefr(Ray ray, Surface s, Intersection intersection) {
		/*
		 * Calcular interseccion recta-plano
		 * N normal
		 * V vertice
		 * P origen del rayo
		 * D direccion del rayo
		 * ti = (N·(V-P))/(N·D)
		 */ 
		Vec3 N = this.normal;
		Vec3 V = this.v0;
		Vec3 P = ray.origin;
		Vec3 D = ray.direction;
		double NdotD = N.dot(D);
		if (NdotD == 0) // Rayo y plano paralelos
			return false;

		double ti = (N.dot(V.sub(P)))/(NdotD);

		if (Math.abs(ti) < ray.minDistance || Math.abs(ti) > ray.maxDistance)
			return false;

		Vec3 Pti = ray.evaluate(ti);
		/*
		 * Calcular el area de los triangulos posibles con v0...v3 y el nuevo punto r(ti)
		 * Si es igual la suma entonces esta dentro 
		 */

		//coord. baricentricas en 3D
		double area = this.area2();
		Vec3 bars = this.barCoords(Pti);
		double areas = bars.x+bars.y+bars.z;

		// Vamos a dar un poco de margen...
		double margen = ray.minDistance;

		if (areas > area + margen) // La interseccion es fuera del triangulo
			return false;

		// Hay interseccion. calcular sus parametros 
		double dist = Pti.sub(ray.origin).length();
		intersection.position = Pti;
		intersection.normal = this.normal;
		intersection.material = this.material;
		intersection.distance = dist;
		intersection.object = this;
		// Texturas
		if (this.hasUVs())
			intersection.uv = this.getUV(Pti);
		if(this.hasBump())
			intersection.normal = this.applyBump(intersection.normal, intersection.uv);

		return true;
	}

	/**
	 * Para las estadísticas.
	 * METODO CREADO POR NOSOTROS
	 */
	public String toString(){
		return "Triangle";
	}

	/**
	 * Descartamos la bounding box para el triángulo.
	 * METODO CREADO POR NOSOTROS
	 */
	public boolean intersectBBox(Ray ray) {
		return true;
	}

	/**
	 * Devuelve las coordenadas de textura del punto en concreto.
	 * METODO CREADO POR NOSOTROS
	 * @param p
	 * @return
	 */
	public Vec3 getUV(Vec3 p){
		Vec3 coords = this.barCoords(p);
		coords.setToScale(1.0/(coords.x+coords.y+coords.z));
		if (v0t == null || v1t == null || v2t == null){
			System.err.println("Este triángulo no tiene coordenadas de textura");
			System.exit(1);
		}
		Vec3 uv = v0t.scale(coords.x);
		uv.setToAdd(v1t.scale(coords.y));
		uv.setToAdd(v2t.scale(coords.z));
		return (uv);
	}
}
