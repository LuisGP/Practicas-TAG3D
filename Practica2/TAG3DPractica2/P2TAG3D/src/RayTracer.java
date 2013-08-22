/**
 * Basic raytracer.
 */
public class RayTracer {
	/**
	 * Scene.
	 */
	public Scene 			scene;

	/**
	 * Max recursion for reflection and refraction.
	 */
	public int				maxRecursion;

	/**
	 * VARIABLE A„ADIDA POR NOSOTROS
	 * Activa o desactiva el muestreo estoc‡stico para el supersampling
	 */
	public boolean			stoch = false;	

	/**
	 * VARIABLE A„ADIDA POR NOSOTROS
	 * Nœmero de rayos lanzados (de la c‡mara a la escena y rebotes solamente)
	 */
	public long				nrays;
	public long				nraystemp;
	public long				nraysaa;
	/**
	 * Threads
	 */
	public RenderThread[]	rtt;
	public ColorImage[] 	cis;
	public int				threads=8;


	/**
	 * Construct a raytracer for a given scene.
	 */	
	public RayTracer(Scene nScene) {
		scene = nScene;
		maxRecursion = 3;
	}


	/**
	 * Reaytrace the scene with one sample per pixel.
	 */
	public ColorImage render() {
		if (this.threads > 1)
			return this.renderMT();
		else
			return this.renderST();
	}



	/**
	 * Reaytrace the scene with one sample per pixel.
	 * Single thread
	 * METODO CREADO POR NOSOTROS
	 */
	public ColorImage renderST() {
		// Llamar pixel a pixel		

		/* DEBUG muestra las normales
		Surface[] normals = ((Mesh)scene.surfaces[0]).displayNormals(); 
		Surface[] nSurfaces = new Surface[normals.length+scene.surfaces.length]; 
		for (int i=0; i<scene.surfaces.length; i++){
			nSurfaces[i] = scene.surfaces[i];
		}
		for (int i=0; i<normals.length; i++){
			nSurfaces[i+(scene.surfaces.length)] = normals[i];
		}
		this.scene.surfaces = nSurfaces; //*/

		this.nrays = 0;
		this.nraystemp = 0;
		Float perstep = 100.0f / this.scene.camera.xResolution; // para el progreso
		int percent = -1; 
		ColorImage ci = new ColorImage(this.scene.camera.xResolution, this.scene.camera.yResolution);
		double u, v;


		for (int i=0; i<this.scene.camera.xResolution; i++){
			for (int j=0; j<this.scene.camera.yResolution; j++){
				Color c = null;
				u = ((double)i)/(double)this.scene.camera.xResolution;
				v = ((double)j)/(double)this.scene.camera.yResolution;

				// DEBUG
				/*
				if (i == 30 && j == 80)
					System.err.println("este peta en el testM"); //*/
				this.nraystemp++;
				c = this.computeColor(this.scene.camera.generateRay(u,v));

				// DEBUG
				/*
				if (i == 30 && j == 80)	
					c = new Color(1,0,0);	//*/

				ci.setColor(i, j, c);
			}
			//* Mostrar porcentaje de progreso
			int offset = (int)(i*perstep);
			if (percent+5 < offset){
				percent+=5;
				System.out.println("Render: ("+offset+"%)");
			} //*/
		}
		System.out.println("Render: (100%)");
		this.nrays = this.nraystemp;

		this.nrays = this.nraystemp;
		return ci;
	}


	/**
	 * Reaytrace the scene with one sample per pixel.
	 * Multithreading
	 * METODO CREADO POR NOSOTROS
	 */
	public ColorImage renderMT() {
		// Llamar pixel a pixel

		/* DEBUG para mostrar las normales
		Surface[] normals = ((Mesh)scene.surfaces[0]).displayNormals(); 
		Surface[] nSurfaces = new Surface[normals.length+scene.surfaces.length]; 
		for (int i=0; i<scene.surfaces.length; i++){
			nSurfaces[i] = scene.surfaces[i];
		}
		for (int i=0; i<normals.length; i++){
			nSurfaces[i+(scene.surfaces.length)] = normals[i];
		}
		this.scene.surfaces = nSurfaces; //*/

		this.nrays = 0;
		ColorImage ci = new ColorImage(this.scene.camera.xResolution, this.scene.camera.yResolution);

		// Lanzar los threads
		cis = new ColorImage[threads];
		this.rtt = new RenderThread[threads];
		for (int i=0; i<this.threads; i++)
			this.rtt[i] = new RenderThread(this, i, false,0);
		for (int i=0; i<this.threads; i++)
			this.rtt[i].start();
		for (int i=0; i<this.threads; i++)
			while (rtt[i].isAlive()){
				try {Thread.sleep(100);} catch (InterruptedException e) {}
			}
		
		// Combinar las im‡genes
		int n=0;
		for (int i=0; i<this.threads; i++){
			n+=cis[i].yResolution;
		}	
		for (int i=0; i<this.threads; i++){
			int ymin = i*(int)(this.scene.camera.yResolution/threads);
			int ymax = cis[i].yResolution;
			for (int x=0; x<this.scene.camera.xResolution; x++){
				for (int y=0; y<ymax; y++){
					ci.setColor(x, y+ymin, this.cis[i].getColor(x, y));
				}
			}
		}
		for (int i=0; i<this.threads; i++)
			this.nrays += rtt[i].nrays;
		return ci;
	}

	/**
	 * Raytrace the scene with nsamples^2 samples oper pixel.
	 */
	public ColorImage renderSuperSampled(int nsamples) {
		if (this.threads > 1)
			return this.renderSuperSampledMT(nsamples);
		else
			return this.renderSuperSampledST(nsamples);
	}


	/**
	 * Raytrace the scene with nsamples^2 samples oper pixel.
	 * Single thread
	 * METODO CREADO POR NOSOTROS
	 */
	public ColorImage renderSuperSampledST(int nsamples) {
		// Lanzar nsamples^2 rayos por pixel 
		this.nraysaa = 0;
		this.nraystemp = 0;
		Float perstep = 100.0f / this.scene.camera.xResolution; // para el progreso
		int percent = -1; 
		ColorImage ci = new ColorImage(this.scene.camera.xResolution, this.scene.camera.yResolution);
		double u, v;
		double step = 1.0 / nsamples;
		double step_2 = step / 2.0;
		for (int i=0; i<this.scene.camera.xResolution; i++){
			for (int j=0; j<this.scene.camera.yResolution; j++){
				Color c = new Color();
				for (int k=0; k<nsamples; k++){
					for (int l=0; l<nsamples; l++){
						double xoff = -step_2;
						double yoff = -step_2;
						if (this.stoch){
							xoff += Math.random() * step;
							yoff += Math.random() * step;
						}
						u = ((double)i+k*step+xoff)/(double)this.scene.camera.xResolution;
						v = ((double)j+l*step+yoff)/(double)this.scene.camera.yResolution;
						this.nraystemp++;
						c.setToAdd(this.computeColor(this.scene.camera.generateRay(u,v)));
					}
				}
				c.setToScale(step*step); 
				ci.setColor(i, j, c);
			}
			//* Mostrar porcentaje de progreso
			int offset = (int)(i*perstep);
			if (percent+5 < offset){
				percent+=5;
				System.out.println("Supersampling: ("+offset+"%)");
			} //*/
		}
		System.out.println("Supersampling: ("+100+"%)");
		this.nraysaa = this.nraystemp;
		return ci;
	}




	/**
	 * Raytrace the scene with nsamples^2 samples oper pixel.
	 * Multithreading
	 * METODO CREADO POR NOSOTROS
	 */
	public ColorImage renderSuperSampledMT(int nsamples) {
		this.nraysaa = 0;
		ColorImage ci = new ColorImage(this.scene.camera.xResolution, this.scene.camera.yResolution);
		cis = new ColorImage[threads];

		this.rtt = new RenderThread[threads];
		for (int i=0; i<this.threads; i++)
			this.rtt[i] = new RenderThread(this, i, true,nsamples);

		for (int i=0; i<this.threads; i++)
			this.rtt[i].start();

		for (int i=0; i<this.threads; i++)
			while (rtt[i].isAlive()){
				try {Thread.sleep(100);} catch (InterruptedException e) {}
			}

		for (int i=0; i<this.threads; i++){
			int ymin = i*(this.scene.camera.yResolution/threads);
			int ymax = cis[i].yResolution;
			for (int x=0; x<this.scene.camera.xResolution; x++){
				for (int y=0; y<ymax; y++){
					ci.setColor(x, y+ymin, this.cis[i].getColor(x, y));
				}
			}
		}

		for (int i=0; i<this.threads; i++)
			this.nraysaa += rtt[i].nraysaa;
		return ci;
	}




	/**
	 * Compute the visible color along the given ray.
	 * Should check to make sure rayDepth is less or equal to the given maximum.
	 */
	public Color computeColor(Ray ray) {
		// Calcular el color final del rayo. 

		// Poner fin a los rebotes
		if (ray.rayDepth > this.maxRecursion)
			return new Color();

		Intersection in = new Intersection();
		if (scene.intersect(ray, in)){
			return computeIllumination(ray, in, false);
		}else{
			return new Color();
		}
	}

	/**
	 * Calcula el color en el caso especial de los rayos refractados
	 * METODO CREADO POR NOSOTROS
	 */
	public Color computeColorRefr(Ray ray, Surface s, Mesh mesh) {
		// Poner fin a los rebotes
		if (ray.rayDepth > this.maxRecursion)
			return new Color();

		Intersection in = new Intersection();
		if (scene.intersectRefr(ray, s, mesh, in)){ // Necesario para refracciones, puede intersectar por dentro
			return computeIllumination(ray, in, true);
		}else{ // Es un plano, no es realista, pero es mas bonito :D
			return computeColor(ray);
		}
	}

	/**
	 * Compute the color of a given intersection point.
	 * Recurse if necessary for reflection.
	 */
	public Color computeIllumination(Ray ray, Intersection intersection, boolean dentro) {
		// El color en un punto concreto de los importantes del rayo. 
		// Si es un material de los que reflejan, lanzar nuevos rayos

		// Sumar el color aportado por cada luz
		Color cfinal = new Color();
		for (int i=0; i<scene.lights.length; i++){
			Light luz = scene.lights[i];

			double indLuz;
			if (intersection.material.toString().charAt(0) == 'E'){ // El material emisivo no recibe sombras
				indLuz = 1;
			}else{ // Sombras
				indLuz = computeShadow(intersection.position, luz);
			}
			if(indLuz > 0){
				if (!intersection.material.hasTexture())
					cfinal.setToAdd(intersection.material.computeDirectLighting(
							intersection.normal, luz.computeLightDirection(intersection.position),
							ray.direction).scale(luz.intensity).scale(indLuz));
				else
					cfinal.setToAdd(intersection.material.computeDirectLighting(
							intersection.normal, luz.computeLightDirection(intersection.position),
							ray.direction,intersection.uv).scale(luz.intensity).scale(indLuz));
			}

			/*
			 *  REFLEJOS
			 */
			if(intersection.material.hasReflection(intersection.normal, ray.direction)){
				// Empezamos con un unico rayo
				Vec3 R = ray.direction.reflect(intersection.normal).normalize();
				Ray rray = new Ray(intersection.position,R);
				rray.minDistance = ray.minDistance;	
				rray.maxDistance = ray.maxDistance-intersection.distance; 
				rray.rayDepth = ray.rayDepth + 1;
				Color cref = computeColor(rray);
				this.nraystemp++;
				cref.setToScale(intersection.material.computeReflection(intersection.normal,rray.direction)); 
				cfinal.setToAdd(cref.scale(luz.intensity));
			}

			/*
			 *  REFRACCION
			 */
			if(intersection.material.hasRefraction(intersection.normal, ray.direction)){
				double refr = intersection.material.getRefraction(intersection.normal, ray.direction);
				Vec3 R;
				if(!dentro)
					R = ray.direction.refract(intersection.normal, ray.refrIdx, refr);
				else
					R = ray.direction.refract(intersection.normal.negate(), refr, 1.0);
				if(R != null){
					Ray rray = new Ray(intersection.position,R);
					rray.minDistance = ray.minDistance;	
					rray.maxDistance = ray.maxDistance-intersection.distance; 
					rray.rayDepth = ray.rayDepth + 1;
					if(!dentro) //No estamos saliendo al aire de nuevo
						rray.refrIdx = refr;
					Color cref = computeColorRefr(rray,intersection.object,intersection.mesh); // Sabemos con quien podria colisionar
					this.nraystemp++; 
					cref.setToScale(intersection.material.computeRefraction(intersection.normal,rray.direction)); 
					cfinal.setToAdd(cref.scale(luz.intensity));
				}
			}
		}
		return cfinal;
	}

	/**
	 * Check if a point P is in shadow given a light.
	 * Returns 0 or 1.
	 */
	public double computeShadow(Vec3 P, Light light) {
		// Ver si cumple light.computeShadowDistance()

		// Sacamos el rayo hacia la luz
		Vec3 area[] = light.computeLightDirectionShadow(P);
		double ilumina = 0;
		double existen = 0;

		for(int i = 0; i < area.length; i++){
			Vec3 ldir = area[i];

			if(ldir != null){
				Ray lray = new Ray(P,ldir);
				lray.maxDistance = light.computeShadowDistance(P);

				// Comprobamos si ese vector "choca" con algœn objeto mientras viaja hacia la luz
				Intersection in = new Intersection();
				if(!scene.intersect(lray, in) || in.material.toString().charAt(0)=='E'){
					ilumina += 1;
				}
				existen += 1;
			}
		}
		if(existen > 0)
			return ilumina/existen;

		return 0;
	}	



	/**
	 * Representaci—n para las estad’sticas.
	 * METODO CREADO POR NOSOTROS
	 */
	public String toString(){
		String out = "PARAMS:\n";
		out += "Num.threads:\t"+this.threads+"\n";
		out += "Stoch.sampling:\t"+((this.stoch) ?"ON\n" :"OFF\n");
		out += "Max.recursion:\t"+this.maxRecursion+"\n";	
		out += "Num.rayos:\t"+this.nrays+"\n";
		out += "Num.rayos.ssamp:\t"+this.nraysaa+"\n";
		return out;
	}
}
