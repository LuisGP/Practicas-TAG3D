/**
 * Base class for object surfaces.
 */
public abstract class Surface {
	/**
	 * Valores mínimos de la bounding box. ATRIBUTO AÑADIDO POR NOSOTROS
	 */
	public Vec3						vmin;
	/**
	 * Valores máximos de la bounding box. ATRIBUTO AÑADIDO POR NOSOTROS
	 */
	public Vec3						vmax;
	/**
	 * Material de la superficie. ATRIBUTO AÑADIDO POR NOSOTROS
	 */
	public Material					material;
	/**
	 * Indice de la textura de bump mapping. ATRIBUTO AÑADIDO POR NOSOTROS
	 */
	public int 					bTexture=-1;

	/**
	 * Contenido de la textura de bump mapping. ATRIBUTO AÑADIDO POR NOSOTROS
	 */
	public ColorImage 			bumpMapping=null;
	/**
	 * Desplazamiento de la textura de bump mapping. ATRIBUTO AÑADIDO POR NOSOTROS
	 */
	public Vec3					toffset=new Vec3();
	/**
	 * Desplazamiento de la textura de bump mapping. ATRIBUTO AÑADIDO POR NOSOTROS
	 */
	public Vec3					tscale=new Vec3(1,1,0);
	

	
	/**
	 * Intersect the given ray with this scene storing the result in intersection and
	 * result true if there was an intersection.
	 */
	abstract boolean intersect(Ray ray, Intersection intersection);

	/**
	 * Intersección de refracción
	 */
	abstract boolean intersectRefr(Ray ray, Surface s, Intersection intersection);
	
	/**
	 * Intersect the given ray with this scene storing the result in intersection and
	 * result true if there was an intersection.
	 * METODO CREADO POR NOSOTROS
	 */
	public boolean intersectBBox(Ray ray){
		double min = ray.minDistance;
		Vec3 vmin = new Vec3(1,1,1).scale(min);
		return this.checkBB(ray, this.vmin.sub(vmin), this.vmax.add(vmin));
	}
	
	/**
	 * Comprueba la intersección con el plano en concreto  
	 */
	protected boolean checkBBPlane(Ray ray, int id, Vec3 vmin, Vec3 vmax){
		/* 
		 * N normal
		 * V vertice
		 * P origen del rayo
		 * D direccion del rayo
		 * t = (N·(V-P))/(N·D)
		 */ 
		Vec3 N = null;
		Vec3 V = null;
		Vec3 P = ray.origin;
		Vec3 D = ray.direction;

		switch(id){
		case 0 : N = new Vec3(0,1,0); V = vmax; break; // TOP
		case 1 : N = new Vec3(0,-1,0);V = vmin; break; // BOTTOM
		case 2 : N = new Vec3(-1,0,0);V = vmin; break; // LEFT
		case 3 : N = new Vec3(1,0,0); V = vmax; break; // RIGHT
		case 4 : N = new Vec3(0,0,-1);V = vmin; break; // FRONT
		case 5 : N = new Vec3(0,0,1); V = vmax; break; // BACK
		}
		double NdotD = N.dot(D);
		if (NdotD == 0) // Rayo y plano paralelos
			return false;
		double t = (N.dot(V.sub(P)))/(N.dot(D));
		
		// Interseccion por detras del origen se descarta (y demasiado lejana)
		if ( t < ray.minDistance || t > ray.maxDistance) 
			return false;

		// Mirar si pertenece a la caja
		Vec3 pt = ray.evaluate(t);
		switch(id){
		case 0 : case 1 : // TOP y BOTTOM
			if (pt.x < vmin.x || pt.z < vmin.z)
				return false;
			if (pt.x > vmax.x || pt.z > vmax.z)
				return false;
			break;
		case 2 : case 3 : // LEFT y RIGHT
			if (pt.y < vmin.y || pt.z < vmin.z)
				return false;
			if (pt.y > vmax.y || pt.z > vmax.z)
				return false;
			break;
		case 4 : case 5 : // FRONT y BACK
			if (pt.x < vmin.x || pt.y < vmin.y)
				return false;
			if (pt.x > vmax.x || pt.y > vmax.y)
				return false;
			break;
		}
				
		// Hay intersección
		return true;
	}


	/**
	 * Comprueba si el rayo atraviesa la BBox.
	 * METODO CREADO POR NOSOTROS
	 * @param ray
	 * @return
	 */
	protected boolean checkBB(Ray ray, Vec3 vmin, Vec3 vmax) {
		/*
		 * Calcular interseccion recta-plano con cada cara de la caja
		 * Si hay intersección, devolver TRUE ya
		 */
		for (int i=0; i<6; i++){
			if (this.checkBBPlane(ray, i, vmin, vmax))
				return true;
		}
		return false;
	}

	
	
	/**
	 * Modifica la normal según la textura del mapa de bultos.
	 * METODO CREADO POR NOSOTROS
	 * @param normal
	 * @param uv2
	 * @return
	 */
	protected Vec3 applyBump(Vec3 normal, Vec3 uv2){
			uv2.x *= this.tscale.x;
			uv2.y *= this.tscale.y;
			uv2.setToAdd(this.toffset);
			Color nmc = this.bumpMapping.getColorUV(uv2.x, uv2.y); // Normal del mapa de bultos
			Vec3 normalAux = new Vec3(2*(nmc.r-.5), -2*(nmc.g-.5), 1); // Direccion de la normal
			// Transformamos esa normal en coordenadas de nuestra escena
			Vec3 normalLocal = new Vec3(normal.x, normal.y, normal.z);
			double xEsc, yEsc, zEsc;
			double hip = normalAux.length();
			double ln = normalLocal.length();
			xEsc = normalLocal.x + ln * (normalAux.x/hip);
			yEsc = normalLocal.y - ln * (normalAux.y/hip);
			zEsc = normalLocal.z;
			Vec3 normalEscena = new Vec3(xEsc, yEsc, zEsc).normalize();
			return normalEscena;
	}
	
	
	/**
	 * True si se aplica un mapa de bultos a la superficie.
	 * METODO CREADO POR NOSOTROS
	 */
	public boolean hasBump(){
		return this.bTexture >= 0;
	}

	/**
	 * True si es necesario generar las coordenadas UV.
	 * METODO CREADO POR NOSOTROS
	 */
	public boolean hasUVs(){
		return this.hasBump() || this.material.hasTexture();
	}

	/**
	 * Asigna el mapa de bultos a la superficie.
	 * METODO CREADO POR NOSOTROS
	 */
	public void setBumpMapping(ColorImage ci){
		this.bumpMapping = ci;
	}

	
}
