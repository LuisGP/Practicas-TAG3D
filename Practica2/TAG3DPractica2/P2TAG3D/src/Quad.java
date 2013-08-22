/**
 * Quad.
 * CLASE CREADA POR NOSOTROS
 */
public class Quad extends Surface {
	/**
	 * Quad vertices.
	 */
	public Vec3					v0;
	public Vec3					v1;
	public Vec3					v2;
	public Vec3					v3;
	/**
	 * Coord. de textura.
	 */
	public Vec3					v0t;
	public Vec3					v1t;
	public Vec3					v2t;
	public Vec3					v3t;
	/**
	 * Triangulos.
	 */
	public Triangle				t0;
	public Triangle				t1;

	
	/**
	 * Inicializa los tri‡ngulos.
	 * Use by the parser.
	 */
	public void initFromParser() {
		this.t0 = new Triangle();
		t0.v0 = this.v0;
		t0.v1 = this.v1;
		t0.v2 = this.v2;
		t0.v0t = this.v0t;
		t0.v1t = this.v1t;
		t0.v2t = this.v2t;
		t0.material = this.material;
		t0.initFromParser();
		t0.bTexture = this.bTexture;
		t0.bumpMapping = this.bumpMapping;
		t0.toffset = this.toffset;
		t0.tscale = this.tscale;
		this.t1 = new Triangle();
		t1.v0 = this.v2;
		t1.v1 = this.v3;
		t1.v2 = this.v0;
		t1.v0t = this.v2t;
		t1.v1t = this.v3t;
		t1.v2t = this.v0t;
		t1.material = this.material;
		t1.bTexture = this.bTexture;
		t1.bumpMapping = this.bumpMapping;
		t1.toffset = this.toffset;
		t1.tscale = this.tscale;
		t1.initFromParser();
		vmin = new Vec3();
		vmin.x = Math.min (Math.min(v0.x, v1.x), Math.min(v2.x, v3.x));
		vmin.y = Math.min (Math.min(v0.y, v1.y), Math.min(v2.y, v3.y));
		vmin.z = Math.min (Math.min(v0.z, v1.z), Math.min(v2.z, v3.z));
		vmax = new Vec3();
		vmax.x = Math.max (Math.max(v0.x, v1.x), Math.max(v2.x, v3.x));
		vmax.y = Math.max (Math.max(v0.y, v1.y), Math.max(v2.y, v3.y));
		vmax.z = Math.max (Math.max(v0.z, v1.z), Math.max(v2.z, v3.z));
	}

	/**
	 * No merece la pena usar bounding box para los quads
	 */
	public boolean intersectBBox(Ray ray){
		return true;
	}

	/**
	 * Intersect the given ray with this scene storing the result in intersection and
	 * result true if there was an intersection.
	 */
	public boolean intersect(Ray ray, Intersection intersection) {
		// Calcular el tri‡ngulo de intersecci—n e interpolar la normal
		boolean isInter0 = false;
		boolean isInter1 = false;
		
		Intersection inter0 = new Intersection();
		isInter0 = this.t0.intersect(ray, inter0);
		Intersection inter1 = new Intersection();
		isInter1 = this.t1.intersect(ray, inter1);
		
		if (!isInter0 && !isInter1) // No hay intersecci—n
			return false;
		
		if (isInter0 && !isInter1){ // interseccion con el primer tri‡ngulo solo
			intersection.set(inter0);
		}else if (!isInter0 && isInter1){ // interseccion con el segundo tri‡ngulo solo
			intersection.set(inter1);
		}else{ // intersectan ambos
			if ( inter0.distance < inter1.distance) // Es mas cercana la primera
				intersection.set(inter0);
			else
				intersection.set(inter1);
		}
		return (isInter0 || isInter1);
	}


	/**
	 * Intersecci—n de refracci—n.
	 * Es la que se produzca con sus tri‡ngulos
	 */
	public boolean intersectRefr(Ray ray, Surface s, Intersection intersection) {
		return intersect(ray, intersection);
	}

	
	/**
	 * Representaci—n de la malla. Para las estad’sticas
	 */
	public String toString(){
		return "Quad";
	}
	
	/**
	 * Asigna el mapa de bultos a la superficie
	 */
	public void setBumpMapping(ColorImage ci){
		this.bumpMapping = ci;
		t0.bumpMapping = ci;
		t1.bumpMapping = ci;
	}
}
