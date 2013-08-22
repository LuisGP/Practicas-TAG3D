import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.Locale;


/**
 * Main command-line entry point.
 * Parses commands, load scenes, renders them and saves results to image files.
 */
public class Main {	
	/**
	 * Main entry point.
	 */
	public static void main(String args[]) {
		if(args.length < 1) {
			System.out.println("usage: Main sceneFilenames");
			return;
		}

		int nsamples = 3;
		
        for (int i = 0; i < args.length; i++) {
            String filename = args[i];
            Calendar hora = new GregorianCalendar();
            System.out.println(hora.get(Calendar.HOUR)+":"+hora.get(Calendar.MINUTE)+":"+hora.get(Calendar.SECOND));
            System.out.println("Reading scene: " + filename);
            Scene scene = loadScene(filename);
            String time1, tpix1;
            String time2, tpix2;
            RayTracer rayTracer = new RayTracer(scene);
            double npixels = rayTracer.scene.camera.xResolution*rayTracer.scene.camera.yResolution;
            NumberFormat nf = NumberFormat.getNumberInstance(Locale.getDefault()); 
            DecimalFormat form = (DecimalFormat)nf; 
            form.applyPattern("##0.00"); 
            System.out.println("Rendering...");
            
            // Render
            Calendar antes = new GregorianCalendar();
            ColorImage image = rayTracer.render(); // Render
            String imageName = filename + ".png";
            Calendar despues = new GregorianCalendar();
           // String imageName = filename +despues.get(Calendar.HOUR)+"-"+antes.get(Calendar.MINUTE)+"-"+antes.get(Calendar.SECOND)+".png";
            System.out.println("Saving image: " + imageName);
            saveImage(imageName,image);
            // Calcular tiempo 
            long diff = despues.getTimeInMillis() - antes.getTimeInMillis();
            Calendar tiempo = new GregorianCalendar();
            tiempo.setTimeInMillis(diff);
            time1 = ((tiempo.get(Calendar.MINUTE)+60*(tiempo.get(Calendar.HOUR)-1)) + "' "+tiempo.get(Calendar.SECOND)+"."+tiempo.get(Calendar.MILLISECOND)+"\""); 
            tpix1 = (""+form.format(diff/npixels));
            // imprimo aqu’ tambiŽn el tiempo para no tener que esperar todo
            System.out.println("Render.Time:\t"+time1);
            System.out.println("Render.Time per pixel(ms):\t"+tpix1);
            
            // Supersampling render
            antes = new GregorianCalendar();
            ColorImage imageSuperSampled = rayTracer.renderSuperSampled(nsamples);
            despues = new GregorianCalendar();
            
            String imageSupersampledName = filename + "supersampled.png";
            System.out.println("Saving image: " + imageSupersampledName);
            saveImage(imageSupersampledName,imageSuperSampled);
            // Calcular tiempo 
            diff = despues.getTimeInMillis() - antes.getTimeInMillis();
            tiempo = new GregorianCalendar();
            tiempo.setTimeInMillis(diff);
            time2 = ((tiempo.get(Calendar.MINUTE)+60*(tiempo.get(Calendar.HOUR)-1)) + "' "+tiempo.get(Calendar.SECOND)+"."+tiempo.get(Calendar.MILLISECOND)+"\""); 
            tpix2 = (""+form.format(diff/npixels));
            
            
            System.out.println("\n-= STATS =-");
            System.out.print(rayTracer.scene);
            System.out.print(rayTracer);
            System.out.println("Num.samples:\t"+nsamples);
            System.out.println("Render.Time:\t"+time1);
            System.out.println("Render.Time.pixel(ms):\t"+tpix1);
            System.out.println("SSampling.Time:\t"+time2);
            System.out.println("SSampling.Time.pixel(ms):\t"+tpix2);
        }
	}

	/**
	 * Save an image to disk.
	 */
	protected static void saveImage(String filename, ColorImage image) {
		try {
			FileFormat.saveImage(filename, image);
		} catch(Exception e) {
			System.out.println("Problem saving image: " + filename);
			System.out.println(e);
            System.exit(1);
		}
	}
	
	/**
	 * Load a scene from file.
	 */
	protected static Scene loadScene(String filename) {
		Scene scene = null;
		try {
            FileFormat p = new FileFormat();
            scene = p.parseXMLScene(filename);
		} catch(Exception e) {
			System.out.println("Problem parsing file: " + filename);
            System.out.println(e);
			System.exit(1);
		}
		return scene;
	}
	
}
