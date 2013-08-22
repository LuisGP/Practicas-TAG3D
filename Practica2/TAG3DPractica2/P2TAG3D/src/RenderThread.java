/**
 * Clase que renderiza media escena
 * CLASE CREADA POR NOSOTROS
 */
public class RenderThread extends Thread {
	RayTracer rt;
	int ind;
	int nraystemp;
	int nrays;
	int nraysaa;
	int nsamps;
	boolean ssamp = false;
	boolean ended = false;
	
	
	/**
	 * Constructor en funci—n de la porci—n de la imagen a renderizar
	 * y el nœmero de muestras
	 * @param rt
	 * @param id
	 * @param ss
	 * @param nsamps
	 */
	public RenderThread(RayTracer rt, int id, boolean ss, int nsamps){
		this.rt = rt;
		this.ind = id;
		this.ssamp = ss;
		this.nsamps = nsamps;
		
	}
	
	/**
	 * Render lanzando un rayo por pixel
	 */
	public void render(){
		Float perstep = 100.0f / rt.scene.camera.xResolution; // para el progreso
		int percent = -1;
		int height = rt.scene.camera.yResolution/rt.threads;
		int ymin = ind*height;
		int ymax = (ind+1)*height;
		if (rt.scene.camera.yResolution % rt.threads != 0 && ind == rt.threads-1){
			ymax+=rt.scene.camera.yResolution % rt.threads;
			height+=rt.scene.camera.yResolution % rt.threads;
		}
		ColorImage ci = new ColorImage(rt.scene.camera.xResolution, height);
		double u, v;
		for (int i=0; i<rt.scene.camera.xResolution; i++){
			for (int j=ymin; j<ymax; j++){
				Color c = null;
				u = ((double)i)/(double)rt.scene.camera.xResolution;
				v = ((double)j)/(double)rt.scene.camera.yResolution;

				// DEBUG
				/*
				if (i == 135 && j == 132)
					System.err.println("este peta en el testM"); //*/
				this.nraystemp++;
				c = rt.computeColor(rt.scene.camera.generateRay(u,v));

				// DEBUG
				/*
				if (i == 135 && j == 132)	
					c = new Color(1,0,0);	//*/

				ci.setColor(i, j-ymin, c);
			}
			//* Mostrar porcentaje de progreso
			int offset = (int)(i*perstep);
			if (percent+5 < offset){
				percent += 5*rt.threads;
				System.out.println("Render id("+ind+"): ("+offset+"%)");
			} //*/
		}
		System.out.println("Render id("+ind+"): (100%)");
		this.nrays = this.nraystemp;
		rt.cis[ind] = ci;
		this.ended = true;
	}

	
	/**
	 * Render lanzando nsamples^2 rayos por pixel
	 * @param nsamples
	 */
	public void renderSuperSampled(int nsamples){
		this.nraysaa = 0;
		this.nraystemp = 0;
		Float perstep = 100.0f / rt.scene.camera.xResolution; // para el progreso
		int percent = -1; 
		int height = rt.scene.camera.yResolution/rt.threads;
		int ymin = ind*height;
		int ymax = (ind+1)*height;
		if (rt.scene.camera.yResolution % rt.threads != 0 && ind == rt.threads-1){
			ymax+=rt.scene.camera.yResolution % rt.threads;
			height+=rt.scene.camera.yResolution % rt.threads;
		}
		ColorImage ci = new ColorImage(rt.scene.camera.xResolution, height);double u, v;
		double step = 1.0 / nsamples;
		double step_2 = step / 2.0;
		for (int i=0; i<rt.scene.camera.xResolution; i++){
			for (int j=ymin; j<ymax; j++){
				Color c = new Color();
				for (int k=0; k<nsamples; k++){
					for (int l=0; l<nsamples; l++){
						double xoff = -step_2;
						double yoff = -step_2;
						if (rt.stoch){
							xoff += Math.random() * step;
							yoff += Math.random() * step;
						}
						u = ((double)i+k*step+xoff)/(double)rt.scene.camera.xResolution;
						v = ((double)j+l*step+yoff)/(double)rt.scene.camera.yResolution;
						this.nraystemp++;
						c.setToAdd(rt.computeColor(rt.scene.camera.generateRay(u,v)));
					}
				}
				c.setToScale(step*step); 
				ci.setColor(i, j-ymin, c);
			}
			//* Mostrar porcentaje de progreso
			int offset = (int)(i*perstep);
			if (percent+5*rt.threads < offset){
				percent += 5*rt.threads;
				System.out.println("Supersampling id("+ind+"): ("+offset+"%)");
			} //*/
		}
		System.out.println("Supersampling id("+ind+"): ("+100+"%)");
		this.nraysaa = this.nraystemp;
		
		rt.cis[ind] = ci;
		this.ended = true;
	}

	
	
	/**
	 * Llamar a uno u otro mŽtodo dependiendo del tipo de thread
	 */
	public void run(){
		if (this.ssamp)
			this.renderSuperSampled(this.nsamps);
		else
			this.render();
	}
}
