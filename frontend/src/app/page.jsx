'use client';

import { useEffect, useState } from 'react';
import { useRouter } from 'next/navigation';

export default function LoginPage() {
  const router = useRouter();
  const [role, setRole] = useState('');
  const [hospitals, setHospitals] = useState([]);
  const [selectedHospital, setSelectedHospital] = useState('');

  useEffect(() => {
    async function fetchHospitals() {
      try {
        const res = await fetch('/api/mongo?collection=Hospital');
        const data = await res.json();
        setHospitals(data?.data || []);
      } catch (err) {
        console.error('Failed to load hospitals', err);
      }
    }

    fetchHospitals();
  }, []);

  const handleLogin = (e) => {
    e.preventDefault();

    if (role === 'super') {
      router.push('/AdminDashboard');
    } else if (role === 'hospital' && selectedHospital) {
      router.push(`/HospitalDashboard?hospitalId=${selectedHospital}`);

    } else {
      alert('Please select a role and hospital (if applicable).');
    }
  };

  return (
    <div className="page d-flex justify-content-center align-items-center vh-100">
      <div className="card shadow-sm rounded-4 border-0" style={{ width: '100%', maxWidth: '400px' }}>
        <div className="card-body p-4">
          <h3 className="text-primary text-center mb-4 fw-bold">HealthApp Login</h3>
          <form onSubmit={handleLogin}>
            {/* Static dummy input fields - ignored by logic */}
            <div className="mb-3">
              <label htmlFor="inputEmail" className="form-label fw-semibold">Email address</label>
              <input type="email" className="form-control" id="inputEmail" placeholder="admin@healthapp.com" />
            </div>
            <div className="mb-4">
              <label htmlFor="inputPassword" className="form-label fw-semibold">Password</label>
              <input type="password" className="form-control" id="inputPassword" placeholder="••••••••" />
            </div>

            <div className="mb-3">
              <label className="form-label fw-semibold">Select Role</label>
              <select className="form-select" value={role} onChange={(e) => setRole(e.target.value)} required>
                <option value="">Choose role</option>
                <option value="super">Super Admin</option>
                <option value="hospital">Hospital Admin</option>
              </select>
            </div>

            {role === 'hospital' && (
              <div className="mb-4">
                <label className="form-label fw-semibold">Select Hospital</label>
                <select
                  className="form-select"
                  value={selectedHospital}
                  onChange={(e) => setSelectedHospital(e.target.value)}
                  required
                >
                  <option value="">Choose hospital</option>
                  {hospitals.map((h, i) => (
                    <option key={i} value={h._id}>
                      {h.name}
                    </option>
                  ))}
                </select>

              </div>
            )}

            <div className="d-grid">
              <button type="submit" className="btn btn-primary fw-semibold">
                Continue
              </button>
            </div>
          </form>
        </div>
        <div className="card-footer text-center text-muted small">
          <span>Login is mocked for now. Role decides redirection.</span>
        </div>
      </div>
    </div>
  );
}
