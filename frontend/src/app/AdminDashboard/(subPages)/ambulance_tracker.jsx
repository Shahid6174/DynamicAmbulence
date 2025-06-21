'use client';

import { useEffect, useState } from 'react';

export default function AmbulanceTracker() {
  const [ambulances, setAmbulances] = useState([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    // Simulate fetching data
    setTimeout(() => {
      setAmbulances([
        {
          id: 'AMB-001',
          driver: 'Shahid Khan',
          status: 'en_route',
          eta: '5 min',
          hospital: 'Aster CMI Hospital',
          location: 'MG Road'
        },
        {
          id: 'AMB-002',
          driver: 'Ravi Verma',
          status: 'picked',
          eta: '12 min',
          hospital: 'Fortis Hospital',
          location: 'Indiranagar'
        },
        {
          id: 'AMB-003',
          driver: 'Anil Joshi',
          status: 'arrived',
          eta: '0 min',
          hospital: 'Manipal Hospital',
          location: 'Whitefield'
        }
      ]);
      setLoading(false);
    }, 1000);
  }, []);

  const getStatusBadge = (status) => {
    switch (status) {
      case 'en_route':
        return <span className="badge bg-warning text-dark">En Route</span>;
      case 'arrived':
        return <span className="badge bg-success">Arrived</span>;
      case 'picked':
        return <span className="badge bg-info text-dark">Picked</span>;
      default:
        return <span className="badge bg-secondary">Unknown</span>;
    }
  };

  return (
    <div className="container py-5">
      <h4 className="fw-bold mb-4 text-primary">
        <i className="bi bi-truck-front-fill me-2"></i> Ambulance Tracker
      </h4>

      <div className="card shadow-sm">
        <div className="card-body table-responsive">
          {loading ? (
            <p className="text-center">Loading ambulances...</p>
          ) : ambulances.length === 0 ? (
            <p className="text-muted text-center">No active ambulances found.</p>
          ) : (
            <table className="table table-hover align-middle">
              <thead className="table-light">
                <tr>
                  <th>Ambulance ID</th>
                  <th>Driver</th>
                  <th>Status</th>
                  <th>ETA</th>
                  <th>Destination</th>
                  <th>Current Location</th>
                </tr>
              </thead>
              <tbody>
                {ambulances.map((amb, index) => (
                  <tr key={index}>
                    <td>{amb.id}</td>
                    <td>{amb.driver}</td>
                    <td>{getStatusBadge(amb.status)}</td>
                    <td>{amb.eta}</td>
                    <td>{amb.hospital}</td>
                    <td>{amb.location}</td>
                  </tr>
                ))}
              </tbody>
            </table>
          )}
        </div>
      </div>
    </div>
  );
}
