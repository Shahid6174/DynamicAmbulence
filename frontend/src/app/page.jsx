'use client';

import Link from 'next/link';
import { useState } from 'react';
import { useRouter } from 'next/navigation';

export default function Home() {
  const [role, setRole] = useState('superadmin');
  const [hospital, setHospital] = useState('');
  const router = useRouter();

  const hospitals = [
    { name: 'Fortis Hospital', slug: 'fortis' },
    { name: 'Aster CMI', slug: 'aster' },
    { name: 'Apollo Hospitals', slug: 'apollo' },
  ];

  const handleSubmit = (e) => {
    e.preventDefault();

    if (role === 'superadmin') {
      router.push('/dashboard/superadmin');
    } else if (role === 'hospitaladmin' && hospital) {
      router.push(`/dashboard/hospital/${hospital}`);
    } else {
      alert('Please select a hospital.');
    }
  };

  return (
    <div id="loginPage" className="page d-flex justify-content-center align-items-center vh-100">
      <div className="card shadow-sm rounded-4 border-0" style={{ width: '100%', maxWidth: '400px' }}>
        <div className="card-body p-4">
          <h3 className="text-primary text-center mb-4 fw-bold">HealthApp Login</h3>
          <form id="loginForm" onSubmit={handleSubmit}>
            <div className="mb-3">
              <label htmlFor="inputEmail" className="form-label fw-semibold">Email address</label>
              <input type="email" className="form-control" id="inputEmail" placeholder="admin@healthapp.com" required />
            </div>

            <div className="mb-3">
              <label htmlFor="roleSelect" className="form-label fw-semibold">Login As</label>
              <select
                className="form-select"
                id="roleSelect"
                value={role}
                onChange={(e) => setRole(e.target.value)}
              >
                <option value="superadmin">Super Admin</option>
                <option value="hospitaladmin">Hospital Admin</option>
              </select>
            </div>

            {role === 'hospitaladmin' && (
              <div className="mb-3">
                <label htmlFor="hospitalSelect" className="form-label fw-semibold">Select Hospital</label>
                <select
                  className="form-select"
                  id="hospitalSelect"
                  value={hospital}
                  onChange={(e) => setHospital(e.target.value)}
                  required
                >
                  <option value="">Choose Hospital</option>
                  {hospitals.map((h, i) => (
                    <option key={i} value={h.slug}>{h.name}</option>
                  ))}
                </select>
              </div>
            )}

            <div className="mb-4">
              <label htmlFor="inputPassword" className="form-label fw-semibold">Password</label>
              <input type="password" className="form-control" id="inputPassword" placeholder="••••••••" required />
            </div>

            <div className="d-grid">
              <button type="submit" className="btn btn-primary fw-semibold">Log In</button>
            </div>
          </form>
        </div>

        <div className="card-footer text-center text-muted small">
          <Link href="/AdminDashboard">
            I am a admin developer
          </Link>
          <br/>
          <Link href="/HospitalDashboard">
            I am a hosptial developer
          </Link>
        </div>
      </div>
    </div>
  );
}
