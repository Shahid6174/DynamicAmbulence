'use client';

import { useEffect, useState } from 'react';
import Link from 'next/link';
import { useRouter } from 'next/navigation';

export default function ManageHospitals() {
  const [data, setData] = useState([]);
  const [loading, setLoading] = useState(true);
  const router = useRouter();

  // Fetch hospitals from MongoDB
  const fetchHospitals = async () => {
    try {
      const res = await fetch('/api/mongo?collection=Hospital');
      const json = await res.json();
      if (res.ok && json.data) {
        setData(json.data);
      } else {
        console.error('Failed to load hospitals:', json?.error);
        setData([]);
      }
    } catch (err) {
      console.error('Network error while fetching:', err);
      setData([]);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchHospitals();
  }, []);

  // Delete hospital
  const handleDelete = async (hospital) => {
    if (!hospital._id) {
      alert('Invalid hospital ID');
      return;
    }

    const confirmDelete = window.confirm(`Delete hospital "${hospital.name}"?`);
    if (!confirmDelete) return;

    try {
      const res = await fetch('/api/mongo?collection=Hospital', {
        method: 'DELETE',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          filter: { _id: typeof hospital._id === 'object' ? hospital._id.$oid || hospital._id : hospital._id },
        }),
      });

      const result = await res.json();

      if (res.ok) {
        alert('✅ Hospital deleted.');
        fetchHospitals(); // Refresh table
      } else {
        alert('❌ Delete failed: ' + result?.error);
      }
    } catch (err) {
      alert('❌ Network error during deletion');
      console.error(err);
    }
  };

  // Edit hospital — go to add_hospital with prefilled query params
  const handleEdit = (hospital) => {
    const params = new URLSearchParams();
    Object.entries(hospital).forEach(([key, val]) => {
      if (typeof val === 'string' || typeof val === 'number') {
        params.append(key, val);
      }
    });
    router.push(`/AdminDashboard/add_hospital?${params.toString()}`);
  };

  return (
    <div className="container-fluid py-4">
      <div className="d-flex justify-content-between align-items-center mb-4">
        <h4 className="fw-bold text-primary mb-0">Manage Hospitals</h4>
        <Link href="/AdminDashboard/add_hospital" className="btn btn-primary">
          <i className="bi bi-hospital me-2" /> Add Hospital
        </Link>
      </div>

      <div className="card shadow-sm">
        <div className="card-body table-responsive">
          {loading ? (
            <p className="text-center">Loading hospitals...</p>
          ) : data.length === 0 ? (
            <p className="text-muted text-center mb-0">No hospitals found.</p>
          ) : (
            <table className="table table-hover align-middle">
              <thead className="table-light">
                <tr>
                  <th>#</th>
                  <th>Name</th>
                  <th>Area</th>
                  <th>Beds</th>
                  <th>Specialization</th>
                  <th>Rating</th>
                  <th className="text-end">Actions</th>
                </tr>
              </thead>
              <tbody>
                {data.map((h, idx) => (
                  <tr key={h._id || idx}>
                    <td>{idx + 1}</td>
                    <td>{h.name}</td>
                    <td>{h.area}</td>
                    <td>{h.beds}</td>
                    <td>{h.specialization}</td>
                    <td>
                      {[...Array(5)].map((_, i) => (
                        <i
                          key={i}
                          className={`bi ${
                            i < h.rating ? 'bi-star-fill' : 'bi-star'
                          } text-warning`}
                        ></i>
                      ))}
                    </td>
                    <td className="text-end">
                      <button
                        className="btn btn-sm btn-outline-success me-2"
                        onClick={() => handleEdit(h)}
                      >
                        <i className="bi bi-pencil-square me-1"></i>
                      </button>
                      <button
                        className="btn btn-sm btn-outline-danger"
                        onClick={() => handleDelete(h)}
                      >
                        <i className="bi bi-trash me-1"></i>
                      </button>
                    </td>
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
