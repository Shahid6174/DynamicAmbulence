import { MapContainer, TileLayer, Marker, Popup } from 'react-leaflet';
import L from 'leaflet';
import 'leaflet/dist/leaflet.css';

// Use your local ambulance icon from the public folder
const ambulanceIcon = new L.Icon({
  iconUrl: '/ambulance-icon.png',
  iconSize: [48, 48],      // Smaller but still visible
  iconAnchor: [24, 48],
  popupAnchor: [0, -48],
});

export default function AmbulanceMap({ ambulances = [] }) {
  const center = ambulances.length
    ? [ambulances[0].lat, ambulances[0].lng]
    : [12.9716, 77.5946];

  return (
    <MapContainer center={center} zoom={12} minZoom={10} style={{ height: '400px', width: '100%', marginTop: 24 }}>
      <TileLayer
        url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
        attribution="&copy; OpenStreetMap contributors"
      />
      {ambulances.map(amb => (
        <Marker key={amb.id} position={[amb.lat, amb.lng]} icon={ambulanceIcon}>
          <Popup>
            <strong>{amb.id}</strong><br />
            Driver: {amb.driver}<br />
            Status: {amb.status}<br />
            ETA: {amb.eta}<br />
            Hospital: {amb.hospital}<br />
            Location: {amb.location}
          </Popup>
        </Marker>
      ))}
    </MapContainer>
  );
}

// Sample data format
const sampleAmbulances = [
  {
    id: "AMB-001",
    driver: "Shahid Khan",
    status: "En Route",
    eta: "5 min",
    hospital: "Aster CMI Hospital",
    location: "MG Road",
    lat: 12.9756,
    lng: 77.6050,
  },
  {
    id: "AMB-002",
    driver: "Ravi Verma",
    status: "Picked",
    eta: "12 min",
    hospital: "Fortis Hospital",
    location: "Indiranagar",
    lat: 12.9719,
    lng: 77.6412,
  },
  {
    id: "AMB-003",
    driver: "Anil Joshi",
    status: "Arrived",
    eta: "0 min",
    hospital: "Manipal Hospital",
    location: "Whitefield",
    lat: 12.9698,
    lng: 77.7499,
  },
];