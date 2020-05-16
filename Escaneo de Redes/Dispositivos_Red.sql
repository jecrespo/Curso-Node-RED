--
-- Database: `Dispositivos_Red`
--
CREATE DATABASE IF NOT EXISTS `Dispositivos_Red` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
USE `Dispositivos_Red`;

-- --------------------------------------------------------

--
-- Table structure for table `dispositivos`
--

CREATE TABLE IF NOT EXISTS `dispositivos` (
  `MAC` varchar(60) NOT NULL,
  `IP` varchar(20) NOT NULL,
  `Vendor` varchar(60) DEFAULT NULL,
  `Nombre` varchar(60) DEFAULT NULL,
  `Red` varchar(40) DEFAULT NULL,
  `Comentarios` varchar(80) DEFAULT NULL,
  `fecha_detectado` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`MAC`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- --------------------------------------------------------

--
-- Table structure for table `historico`
--

CREATE TABLE IF NOT EXISTS `historico` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `fecha` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `MAC` varchar(40) NOT NULL,
  `IP_Antigua` varchar(40) NOT NULL,
  `IP_Nueva` varchar(40) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
