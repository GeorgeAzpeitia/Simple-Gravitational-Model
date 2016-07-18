import Graphics.UI.GLUT
import Data.IORef
import Unsafe.Coerce(unsafeCoerce)
import GHC.Float(double2Float)
import Debug.Trace
import System.Random
doubleToGF = unsafeCoerce . double2Float :: Double -> GLfloat
data Planet = Planet { x :: Double
                     , y :: Double
                     , z :: Double
                     , vx :: Double
                     , vy :: Double
                     , vz :: Double
                     , colorR :: Double
                     , colorG :: Double
                     , colorB :: Double
                     , mass :: Double
                     , radius :: Double
                     , star :: Bool
                     , coords :: [(Double, Double)]
                     , removed :: Bool
                     } deriving (Show, Eq)

sol = Planet 0 0 0 0 0 0 1 1 0 5000000 100 True [(0,0)] False
planetOne = Planet (-300) 200 0 (0) (-10) 0 0.03 0 0.5 100 10 False [(-300,200)] False
grav_constant = 0.00667
step = 1
deltaT = 1 / step
main :: IO ()
main = do
  (_progName, _args) <- getArgsAndInitialize
  initialWindowSize $= Size 1000 1000
  initialWindowPosition $= Position 100 100
  _window <- createWindow "Grav -- 's' to add planets 'S' to add stars"
  planets <- newIORef ([sol , planetOne]:: [Planet])
  matrixMode $= Projection
  loadIdentity
  ortho (-1000) 1000 (-1000) 1000 0 1
  keyboardMouseCallback $= Just (keyboardMouse planets)
  displayCallback $= (display planets)
  updatePlanets planets
  timer planets
  mainLoop

keyboardMouse :: IORef [Planet] -> KeyboardMouseCallback
keyboardMouse planets key _ _ _ = case key of
  (Char 'S') -> addStars planets
  (Char 's') -> addPlanets planets
  _ -> return ()

addStars :: IORef [Planet] -> IO()
addStars planetsIO = do
  planets <- readIORef planetsIO
  updatedPlanets <- (addStarsRec 5 (return planets))
  writeIORef planetsIO updatedPlanets
  where addStarsRec :: Int -> IO [Planet] -> IO [Planet]
        addStarsRec 0 p = p
        addStarsRec i p = do
                        g <- newStdGen
                        rx <- randomRIO(-1000.0, 1000.0)::IO Double
                        ry <- randomRIO(-1000.0, 1000.0)::IO Double 
                        rz <- randomRIO(-1000.0, 1000.0)::IO Double
                        rvx <- randomRIO(-10.0, 10.0)::IO Double
                        rvy <- randomRIO(-10.0, 10.0)::IO Double
                        rvz <- randomRIO(-10.0, 10.0)::IO Double
                        planets <- p
                        r <- addStarsRec (i-1) (return ((Planet rx ry rz 
                                                 rvx rvy rvz
                                                 1 1 0
                                                (5000000) (80) 
                                                True [(rx, ry)] False):planets))
                        return r
addPlanets :: IORef [Planet] -> IO()
addPlanets planetsIO = do
  planets <- readIORef planetsIO
  updatedPlanets <- (addPlanetsRec 5 (return planets))
  writeIORef planetsIO updatedPlanets
  where addPlanetsRec :: Int -> IO [Planet] -> IO [Planet]
        addPlanetsRec 0 p = p
        addPlanetsRec i p = do
                        g <- newStdGen
                        rx <- randomRIO(-1000.0, 1000.0)::IO Double
                        ry <- randomRIO(-1000.0, 1000.0)::IO Double 
                        rz <- randomRIO(-1000.0, 1000.0)::IO Double
                        rvx <- randomRIO(-10.0, 10.0)::IO Double
                        rvy <- randomRIO(-10.0, 10.0)::IO Double
                        rvz <- randomRIO(-10.0, 10.0)::IO Double
                        rcolorR <- randomRIO(0.0, 1.0)::IO Double
                        rcolorG <-randomRIO(0.0, 1.0)::IO Double
                        rcolorB <-randomRIO(0.0, 1.0)::IO Double
                        rmass <- randomRIO(0.0, 500):: IO Double
                        planets <- p
                        r <- addPlanetsRec (i-1) (return ((Planet rx ry rz 
                                                 rvx rvy rvz
                                                 rcolorR rcolorG rcolorB
                                                (rmass + 500) ((rmass + 500) * 20 / 1000) 
                                                True [(rx, ry)] False):planets))
                        return r

display :: IORef [Planet] -> DisplayCallback
display planetsIO = do
  let color3f r g b = color $ Color3 (doubleToGF r) (doubleToGF g) (doubleToGF b)
      vertex3f x y z = vertex $ Vertex3 (doubleToGF x) (doubleToGF y) (doubleToGF z)
  clear [ColorBuffer]
  pointSmooth $= Enabled
  planets <- readIORef planetsIO
  mapM_ renderPlanets planets
  mapM_ renderTrail planets
  flush

renderPlanets :: Planet -> IO()
renderPlanets p = do
    let color3f r g b = color $ Color3 (doubleToGF r) (doubleToGF g) (doubleToGF b)
    let vertex3f x y z = vertex $ Vertex3 (doubleToGF x) (doubleToGF y) (doubleToGF z)
    pointSize $= (doubleToGF (radius p))
    renderPrimitive Points $ do
        color3f (colorR p) (colorG p) (colorB p)
        vertex3f (x p) (y p) 0.0

renderTrail :: Planet -> IO()
renderTrail p = do
    let color3f r g b = color $ Color3 (doubleToGF r) (doubleToGF g) (doubleToGF b)
    let vertex3f x y z = vertex $ Vertex3 (doubleToGF x) (doubleToGF y) (doubleToGF z)
    color3f 1 1 1
    renderPrimitive Lines $ do
        mapM_ (\ (x, y) -> vertex3f x y 0) (coords p)


updatePlanets :: IORef [Planet] -> IO()
updatePlanets planetsIO = do
    planets <- readIORef planetsIO
    planets <-  return (map updatePosition planets)
    updatedPlanets <- return $ filter(\p -> not(removed p)) (map (\ x -> updateVelocity x planets) planets)
    writeIORef planetsIO updatedPlanets
    

updatePosition :: Planet -> Planet
updatePosition p = Planet ((x p) + (vx p) * deltaT) 
                          ((y p) + (vy p) * deltaT) 
                          ((z p) + (vz p) * deltaT) 
                          (vx p) (vy p) (vz p) 
                          (colorR p) (colorG p) (colorB p) 
                          (mass p) (radius p) (star p) (coords p) False

updateVelocity :: Planet -> [Planet] -> Planet
updateVelocity p1 [] = (Planet (x p1) (y p1) (z p1) 
                      (vx p1) (vy p1) (vz p1) 
                      (colorR p1) (colorG p1) (colorB p1) 
                      (mass p1) (radius p1) (star p1) ((coords p1) ++ [(x p1, y p1)]) (removed p1))
updateVelocity p1 (p2:ps)
    | (distance p1 p2) == (0.0) = updateVelocity p1 ps
    | (distance p1 p2) <= (radius p1 + radius p2) = if (mass p1 <= mass p2)
                                                  then(Planet (x p1) (y p1) (z p1) 
                                                      (vx p1) (vy p1) (vz p1) 
                                                      (colorR p1) (colorG p1) (colorB p1) 
                                                      (mass p1 + mass p2) (radius p1) (star p1) (coords p1) (True))
                                                  else updateVelocity (Planet (x p1) (y p1) (z p1) 
                                                      (accelX p1 p2) (accelY p1 p2) (accelZ p1 p2) 
                                                      (colorR p1) (colorG p1) (colorB p1) 
                                                      (mass p1) (radius p1) (star p1) (coords p1) (False)) ps
    | otherwise = updateVelocity (Planet (x p1) (y p1) (z p1) 
                              (accelX p1 p2) (accelY p1 p2) (accelZ p1 p2) 
                              (colorR p1) (colorG p1) (colorB p1) 
                              (mass p1) (radius p1) (star p1) (coords p1) (False)) ps

distance :: Planet -> Planet -> Double
distance p1 p2 = sqrt((((x p2) - (x p1)) * ((x p2) - (x p1))) 
                    + (((y p2) - (y p1)) * ((y p2) - (y p1)))
                    + (((z p2) - (z p1)) * ((z p2) - (z p1))))

force :: Planet -> Planet -> Double
force p1 p2 = (grav_constant * (mass p1) * (mass p2)) / (distance p1 p2) ^ 2

accel :: Planet -> Planet -> Double
accel p1 p2 = force p1 p2 / mass p1

accelX :: Planet -> Planet -> Double                
accelX p1 p2 = (vx p1) + (accel p1 p2) * (unitX p1 p2) * deltaT
unitX :: Planet -> Planet -> Double
unitX p1 p2 = ((x p2) - (x p1)) / (distance p1 p2)
accelY :: Planet -> Planet -> Double
accelY p1 p2 = (vy p1) + ((force p1 p2) / (mass p1)) * (unitY p1 p2) * deltaT
unitY :: Planet -> Planet -> Double
unitY p1 p2 = ((y p2) - (y p1)) / (distance p1 p2)
accelZ :: Planet -> Planet -> Double
accelZ p1 p2 = (vz p1) + ((force p1 p2) / (mass p1)) * (unitZ p1 p2) * deltaT
unitZ :: Planet -> Planet -> Double
unitZ p1 p2 = ((z p2) - (z p1)) / (distance p1 p2)

timer :: IORef [Planet] -> TimerCallback
timer planetsIO = do
    timerRec step planetsIO
    postRedisplay Nothing
    --planets <- readIORef planetsIO
    
    addTimerCallback 30 (timer planetsIO)
    where timerRec 0 _ = return ()
          timerRec step planetsIO = do
            updatePlanets planetsIO
            timerRec (step -1) planetsIO